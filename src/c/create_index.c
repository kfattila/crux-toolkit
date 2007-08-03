/*****************************************************************************
 * \file create_index
 * AUTHOR: Chris Park
 * CREATE DATE: August 10 2006
 * DESCRIPTION: Given a protein fasta sequence database as input, generate crux_index files
 *              that contain list of peptides in 
 *              the database that meet certain criteria (e.g. mass, length, trypticity) as output.
 * REVISION: 
 ****************************************************************************/
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include "carp.h"
#include "peptide.h"
#include "peptide_src.h"
#include "protein.h"
#include "database.h"
#include "parse_arguments.h"
#include "index.h"
#include "protein_index.h"
#include "parameter.h"

/**
 * when wrong command is seen carp, and exit
 */
void wrong_command(char* arg){
  char* usage = parse_arguments_get_usage("create_index");
  carp(CARP_FATAL, "incorrect argument %s", arg);
  fprintf(stderr, "%s", usage);
  free(usage);
  exit(1);
}

/**
 * progress indicator, displays a spinning | to standout
 */
void show_progress(int* num){
  putc('\b', stderr);
  if(*num / 150 == 37){
    putc('|', stderr);
    fflush(stderr);
  }
  else if(*num / 150 == 74){
    putc('/', stderr);
    fflush(stderr);
  }
  else if(*num / 150 == 111){
    putc('-', stderr);
    fflush(stderr);
  }
  else if(*num / 150 == 149){
    putc('\\', stderr);
    fflush(stderr);
    *num = 0;
    return;
  }
  ++*num;
}
      
int main(int argc, char** argv){

  // connect various signals to our clean-up function
  signal( SIGTERM, clean_up );
  signal( SIGINT, clean_up );
  signal( SIGQUIT, clean_up );
  signal( SIGHUP, clean_up ); 

  /* Set default values for any options here */
  double min_mass = 200;
  double max_mass = 7200;
  double mass_range = 1;  
  int min_length = 6;
  int max_length = 50;
  char* cleavages = "tryptic"; 
  char* isotopic_mass = "average" ;
  int  verbosity = CARP_INFO;
  char* redundancy = "redundant";
  char* binary_fasta_file = NULL;
  char* parameter_file = NULL;
  
  MASS_TYPE_T mass_type = AVERAGE;
  PEPTIDE_TYPE_T peptide_type = TRYPTIC;
  int missed_cleavages = FALSE;
  char * in_file = NULL;
  /*const*/ char * error_message;
  int result = 0;
  BOOLEAN_T is_unique = FALSE;

  /* Define optional command line arguments */ 

  parse_arguments_set_opt(
    "mass-range", 
    "The mass range contained in each index file.",
    (void *) &mass_range, 
    DOUBLE_ARG);

  parse_arguments_set_opt(
    "min-mass", 
    "The minimum mass of the peptides to put in index file.", 
    (void *) &min_mass, 
    DOUBLE_ARG);

  parse_arguments_set_opt(
    "max-mass", 
    "The maximum mass of the peptides to output in index file.", 
    (void *) &max_mass, 
    DOUBLE_ARG);

  parse_arguments_set_opt(
    "min-length", 
    "The minimum length of the peptides to output in index file.",
    (void *) &min_length, 
    INT_ARG);

  parse_arguments_set_opt(
    "max-length", 
    "The maximum length of the peptides to output in index file. maximum limit = 255.",
    (void *) &max_length, 
    INT_ARG);

  parse_arguments_set_opt(
    "cleavages", 
    "Type of cleavages to allow. tryptic|partial|all.", 
    (void *) &cleavages, 
    STRING_ARG);

  parse_arguments_set_opt(
    "missed-cleavages", 
    "Allow missed cleavage sites with in a peptide. ",
    (void *) &missed_cleavages, 
    FLAG_ARG);
 
  parse_arguments_set_opt(
    "isotopic-mass", 
    "Specify the type of isotopic masses to use when calculating the peptide mass. average|mono.",
    (void *) &isotopic_mass, 
    STRING_ARG);

  parse_arguments_set_opt(
    "verbosity", 
    "Specify the verbosity of the current processes from 0-100.",
    (void *) &verbosity, 
    INT_ARG);

  parse_arguments_set_opt(
    "redundancy", 
    "Specify whether peptides that come from different proteins yet with identical sequences should appear on separate lines or on the same line. redundant|unique.",
    (void *) &redundancy, 
    STRING_ARG);

  parse_arguments_set_opt(
    "parameter-file",
    "The crux parameter file to parse parameter from.",
    (void *) &parameter_file,
    STRING_ARG); 
  
  /* Define required command line arguments */
  parse_arguments_set_req(
    "protein input filename", 
    "The name of the file (in fasta format) from which to parse proteins.", 
    (void *) &in_file, STRING_ARG);
  

  /* Parse the command line */
  if (parse_arguments(argc, argv, 0)) {
    PEPTIDE_CONSTRAINT_T* constraint;
    INDEX_T* crux_index;
    
    //set verbosity
    if(CARP_FATAL <= verbosity && verbosity <= CARP_MAX){
      set_verbosity_level(verbosity);
    }
    else{
      wrong_command("verbosity");
    }
    
    //parse and update parameters
    parse_update_parameters(parameter_file);

    //parameters are now confirmed, can't be changed
    parameters_confirmed();
    
    /******* All parameters must be taken through get_*_parameter() method ******/

    //FIXME may add additional types such as non-tryptic or partially-tryptic
    if(strcmp(get_string_parameter_pointer("cleavages"), "all")==0){
      peptide_type = ANY_TRYPTIC;
    }
    else if(strcmp(get_string_parameter_pointer("cleavages"), "tryptic")==0){
      peptide_type = TRYPTIC;
    }
    else if(strcmp(get_string_parameter_pointer("cleavages"), "partial")==0){
      peptide_type = PARTIALLY_TRYPTIC;
    }
    else{
      wrong_command(cleavages);
    }
        
    //check if maximum length is with in range <= 255
    max_length = get_int_parameter("max-length",50);
    if(max_length > 255){
      carp(CARP_FATAL, "maximum length:%d over limit 255.", max_length);
      exit(1);
    }
    
    mass_range = get_double_parameter("mass-range", 1);
    if(compare_float(mass_range, 0) == 0){
      carp(CARP_FATAL, "mass_range:%d must be greater than 0.", mass_range);
      exit(1);
    }
    
    //determine isotopic mass option
    if(strcmp(get_string_parameter_pointer("isotopic-mass"), "average")==0){
      mass_type = AVERAGE;
    }
    else if(strcmp(get_string_parameter_pointer("isotopic-mass"), "mono")==0){
      mass_type = MONO;
    }
    else{
      wrong_command(isotopic_mass);
    }
   
    //determine redundancy option
    if(strcmp(get_string_parameter_pointer("redundancy"), "redundant")==0){
      is_unique = FALSE;
    }
    else if(strcmp(get_string_parameter_pointer("redundancy"), "unique")==0){
      is_unique = TRUE;
    }
    else{
      wrong_command(redundancy);
    }
    
    //check if input file exist
    if(access(in_file, F_OK)){
      carp(CARP_FATAL, "The file \"%s\" does not exist (or is not readable, or is empty).", in_file);
      exit(1);
    }
    
    //set other parameters to final
    min_mass = get_double_parameter("min-mass", 200);
    max_mass = get_double_parameter("max-mass", 2400);
    min_length = get_int_parameter("min-length", 6);
    max_length = get_int_parameter("max-length", 50);
    missed_cleavages = get_boolean_parameter("missed-cleavages", FALSE);

    //peptide constraint
    constraint = 
      new_peptide_constraint(peptide_type, min_mass, max_mass, min_length, max_length, missed_cleavages, mass_type);
 
    
    //create new index object
    crux_index = 
      new_index(in_file,
                constraint,
                mass_range,                
                is_unique,
                FALSE
                );
    
    //create crux_index files
    if(!create_index(crux_index)){
      die("failed to create index");
    }
    
    //free index(frees constraint together);
    free_index(crux_index);     
    free(binary_fasta_file);
    exit(0);
  } 
  else {
    char* usage = parse_arguments_get_usage("create_index");
    result = parse_arguments_get_error(&error_message);
    fprintf(stderr, "Error in command line. Error # %d\n", result);
    fprintf(stderr, "%s\n", error_message);
    fprintf(stderr, "%s", usage);
    free(usage);
  }
  exit(0);
}

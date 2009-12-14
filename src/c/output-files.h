/**
 * \file output-files.h
 */
/*
 * FILE: output-files.h
 * AUTHOR: Barbara Frewen
 * CREATE DATE: Aug 24, 2009
 * PROJECT: crux
 * DESCRIPTION: A class description for handling all the various
 * output files, excluding parameter and log files.  The filenames,
 * locations and overwrite status would be taken from parameter.c.
 */
#ifndef OUTPUT_FILES_H
#define OUTPUT_FILES_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include "carp.h"
#include "parameter.h"
#include "objects.h"
#include "match_collection.h"

class OutputFiles{

 public:
  OutputFiles(const char* program_name);///< name of the crux function
                                      ///(e.g. search)
  ~OutputFiles();
  void writeHeaders(int num_proteins = 0);
  void writeMatches(/*const*/ MATCH_COLLECTION_T* matches,
                    /*const*/ MATCH_COLLECTION_T** decoy_matches_array,
                    int num_decoys,
                    SCORER_TYPE_T rank_type = XCORR,
                    SPECTRUM_T* spectrum = NULL);
  void updateHeaders(int spectrum_count);

 private:
  BOOLEAN_T createFiles(FILE*** file_array_ptr,
                        const char* output_dir,
                        const char* fileroot,
                        const char* command_name,
                        const char* extension,
                        BOOLEAN_T overwrite);
  void printMatchesTab(
    MATCH_COLLECTION_T*  target_matches, ///< from real peptides
    MATCH_COLLECTION_T** decoy_matches_array,  
                           ///< array of collections from shuffled peptides
    SCORER_TYPE_T rank_type,
    SPECTRUM_T* spectrum = NULL);

  void printMatchesPsm(
    MATCH_COLLECTION_T*  target_matches, ///< from real peptides
    MATCH_COLLECTION_T** decoy_matches_array);  
                           ///< array of collections from shuffled peptides
                       
  void printMatchesSqt(
    MATCH_COLLECTION_T*  target_matches, ///< from real peptides
    MATCH_COLLECTION_T** decoy_matches_array,  
                           ///< array of collections from shuffled peptides
  SPECTRUM_T* spectrum = NULL);

  int num_files_;         ///< num files in each array
  FILE** psm_file_array_; ///< array of .csm files
  FILE** tab_file_array_; ///< array of .txt files
  FILE** sqt_file_array_; ///< array of .sqt files
  int matches_per_spec_;   ///< print this many matches per spec
};










#endif //OUTPUT_FILES_H

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */






























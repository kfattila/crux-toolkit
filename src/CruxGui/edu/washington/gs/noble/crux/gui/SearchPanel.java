package edu.washington.gs.noble.crux.gui;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.logging.Logger;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JPanel;

@SuppressWarnings("serial")
class SearchPanel extends JPanel implements ItemListener {

	private static Logger logger = 
		Logger.getLogger("edu.washington.gs.noble.crux.gui");
	private final JCheckBox runToolCheckBox = new JCheckBox("Run this tool");
	private ProteinDBPanel proteinDBPanel = null;
	private SpectraPanel spectraPanel = null;
 	private EnzymePanel enzymePanel = null;
	private MassPanel massPanel = null;
	private DigestPanel digestPanel = null;
	private final JCheckBox missedCleavages = new JCheckBox("Allow missed-cleavages");
	private CruxComponentButton button;
	private CruxAnalysisModel model = null;
	
	public SearchPanel(CruxAnalysisModel model, final CruxComponentButton button) {
		super();
		this.button = button;
		this.model = model;
		setBackground(Color.white);
		setLayout(new BoxLayout(this, BoxLayout.PAGE_AXIS));
		proteinDBPanel = new ProteinDBPanel(model);
		spectraPanel = new SpectraPanel(model);
		enzymePanel = new EnzymePanel(model);
		massPanel = new MassPanel(model);
		digestPanel = new DigestPanel(model);
		runToolCheckBox.setBackground(Color.white);
		add(runToolCheckBox);
		add(Box.createRigidArea(new Dimension(0,12)));
		add(proteinDBPanel);
		add(Box.createRigidArea(new Dimension(0,12)));
		add(spectraPanel);
		add(Box.createRigidArea(new Dimension(0,12)));
		add(massPanel);
		add(Box.createRigidArea(new Dimension(0,12)));
		add(digestPanel);
		add(Box.createRigidArea(new Dimension(0,12)));
		add(enzymePanel);
		add(Box.createRigidArea(new Dimension(0,12)));
		missedCleavages.setBackground(Color.white);
		missedCleavages.setAlignmentX(Component.LEFT_ALIGNMENT);
		add(missedCleavages);
		runToolCheckBox.addItemListener(new RunComponentChangeListener());
		missedCleavages.addItemListener(new MissedCleavagesChangeListener());
		updateFromModel();
		setVisible(false);

	}
	
	private void updateFromModel() {
		runToolCheckBox.setSelected(model.getRunSearchForMatches());
		massPanel.updateFromModel();
		digestPanel.updateFromModel();
		enzymePanel.updateFromModel();
		missedCleavages.setSelected(model.getAllowMissedCleavages());
	}
	
	public void itemStateChanged(final ItemEvent event) {
		logger.info("User selected search component.");
		updateFromModel();
		if (((CruxComponentButton) event.getSource()).isSelected() == true) {
			boolean runIndex = model.getRunCreateIndex();
			proteinDBPanel.setVisible(!runIndex);
			// Some components are only shown if we aren't creating an index
			massPanel.setVisible(!runIndex);
			digestPanel.setVisible(!runIndex);
			enzymePanel.setVisible(!runIndex);
			missedCleavages.setVisible(!runIndex);
			setVisible(true);
		}
		else {
			setVisible(false);
		}
	}
	
	class RunComponentChangeListener implements ItemListener {
		public void itemStateChanged(final ItemEvent event) {
			boolean checked = ((JCheckBox) event.getSource()).isSelected();
			button.setSelectedToRun(checked);
			model.setRunCreateIndex(checked);
		}	
	}
	
	class MissedCleavagesChangeListener implements ItemListener {
		public void itemStateChanged(final ItemEvent event) {
			boolean checked = missedCleavages.isSelected();
			model.setAllowMissedCleavages(checked);
		}
	}	
}
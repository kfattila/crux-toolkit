package edu.washington.gs.noble.crux.gui;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Insets;
import java.awt.geom.Ellipse2D;
import java.util.logging.Logger;

import javax.swing.Icon;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JToggleButton;

/**
 * CruxCompnentCheckButton extends the SWING JToggleButton class.
 * A checkbox and a panel containing an icon are embeded in a
 * standard JTogglbutton. The checkbox is used to indicate whether
 * the corresponding Crux component should be run as part of the
 * analysis. The icon is a colored bullet indicating the status of 
 * Crux component in the analysis: 
 * <ul>
 * <li>NONE</li>
 * <li>NOT_RUN (white)</li>
 * <li>COMPLETED (green)</li>
 * <li>FAILED (red)</li>
 * </ul>
 * 
 * @author Charles E. Grant
 *
 */
class CruxComponentButton extends JToggleButton {
	
	public enum Status {
		NOT_RUN, COMPLETED, FAILED
	};

	private final JCheckBox processCheckBox = new JCheckBox();
	private final JLabel statusIconLabel = new JLabel();
	private Status status = Status.NOT_RUN;

	private static Logger logger = Logger.getLogger("edu.washington.gs.noble.crux.gui");

	private static Color getStatusColor(Status state){
		switch(state) {
		case COMPLETED:
			return Color.green;
		case FAILED:
			return Color.red;
		case NOT_RUN:
			return Color.white;
		default:
			return null;	
		}
	}
	
	class StatusIcon implements Icon {
	
		private final int width = 10;
		private final int height = 10;
		
		public int getIconWidth() { return width; }
		public int getIconHeight() { return height; }
		
		public void paintIcon(Component c, Graphics g, int x, int y) {
			Graphics2D g2 = (Graphics2D) g;
			Ellipse2D circle = new Ellipse2D.Double(x, y, width, height);
			g2.setPaint(getStatusColor(status));
			g2.fill(circle);
			g2.setPaint(Color.black);
			g2.draw(circle);
		}
	}
	
	public CruxComponentButton(String text) {
		super(text);
		setLayout(new BorderLayout());
		this.setMargin(new Insets(0,0,0,0));
		processCheckBox.setMargin(new Insets(0, 0, 0, 0));
		processCheckBox.setContentAreaFilled(false);
		processCheckBox.setEnabled(false);
		statusIconLabel.setIcon(new StatusIcon());
		add(processCheckBox, BorderLayout.WEST);
		add(statusIconLabel, BorderLayout.EAST);
	}
	
	public void setSelectedToRun(boolean run) {
		this.processCheckBox.setSelected(run);
	}
	
	public boolean isSelectedToRun() {
		return this.processCheckBox.isSelected();
	}
	
	public void setStatus(Status status) {
		this.status = status;
	}
	
	public Status getStatus() {
		return status;
	}
	
}
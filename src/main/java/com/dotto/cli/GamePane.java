package com.dotto.cli;

import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.RenderingHints;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JPanel;

import com.dotto.cli.util.Config;
import com.dotto.cli.util.GameLoop;
import com.dotto.cli.view.Boot;
import com.dotto.cli.view.View;

/**
 * Drawing surface for views and view manager.
 * 
 * @author lite20 (Ephraim Bilson)
 * @author SoraKatadzuma
 */
@SuppressWarnings("serial")
public class GamePane extends JPanel implements MouseListener, KeyListener {
    /** Dedicated object for running rendering calls. */
    public GameLoop renderLoop;
    /** Dedicated object for running update calls. */
    public GameLoop updateLoop;
    /** Current active view in the game pane. */
    public View view;

    /**
     * Constructs a new {@code GamePane} object.
     */
    public GamePane() {
        setOpaque(true);
        
        // The starting view of the game.
        view = new Boot();

        // Assigning the graphics rendering loop for this Pane.
        renderLoop = new GameLoop(
            delta -> {
                repaint();
            }, 100
        );

        // Assigning the game's updating loop this Pane.
        updateLoop = new GameLoop(
            delta -> {
                view.update(delta);
            }, 60
        );
    }

    /**
     * Inherited method.
     * 
     * @param g The {@code Graphics} being painted to the view.
     * @see javax.swing.JPanel#paintComponent(java.awt.Graphics)
     */
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);

        Graphics2D g2 = (Graphics2D) g;
        if (Config.ANTIALIAS) {
            RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_TEXT_ANTIALIASING,
                RenderingHints.VALUE_TEXT_ANTIALIAS_ON
            );

            g2.setRenderingHints(rh);
        }

        g2.setRenderingHint(
            RenderingHints.KEY_STROKE_CONTROL, RenderingHints.VALUE_STROKE_PURE
        );

        g2.setRenderingHint(
            RenderingHints.KEY_INTERPOLATION,
            RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR
        );

        view.draw(g2);
    }

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.MouseListener#mousePressed(java.awt.event.MouseEvent)
     */
    @Override
    public void mousePressed(MouseEvent e) {
        view.mouseDown(e);
    }

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.MouseListener#mouseReleased(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseReleased(MouseEvent e) {
        view.mouseUp(e);
    }

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.KeyListener#keyPressed(java.awt.event.KeyEvent)
     */
    @Override
    public void keyPressed(KeyEvent e) {
        view.keyDown(e);
    }

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.KeyListener#keyReleased(java.awt.event.KeyEvent)
     */
    @Override
    public void keyReleased(KeyEvent e) {
        view.keyUp(e);
    }

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.KeyListener#keyTyped(java.awt.event.KeyEvent)
     */
    @Override
    public void keyTyped(KeyEvent e) {}

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.MouseListener#mouseClicked(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseClicked(MouseEvent e) {}

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.MouseListener#mouseEntered(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseEntered(MouseEvent e) {}

    /**
     * Inherited method.
     * 
     * @param e The event being performed.
     * @see java.awt.event.MouseListener#mouseExited(java.awt.event.MouseEvent)
     */
    @Override
    public void mouseExited(MouseEvent e) {}
}

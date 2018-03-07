package com.dotto.cli.view;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import com.dotto.cli.Core;
import com.dotto.cli.util.BeatStreamReader;
import com.dotto.cli.util.Config;
import com.dotto.cli.util.asset.Beat;
import com.dotto.cli.util.asset.BeatMap;
import com.dotto.cli.util.asset.MapData;
import com.dotto.cli.util.manager.MapConfigure;

/**
 * TODO: write class description.
 * 
 * @author lite20 (Ephraim Bilson)
 * @author SoraKatadzuma
 */
public class Track implements View {
    /** A manual serial id, instead of the normal Java serailID. */
    public static int ID = 3;

    public double xOffset = 0;
    public double yOffset = 0;
    public double xAccel = 0;
    public double yAccel = 0;
    public double speed = 5;
    public double glideFactor = 0.5;

    public boolean UP = false;
    public boolean DOWN = false;
    public boolean LEFT = false;
    public boolean RIGHT = false;

    public BeatMap map;

    private final String path;

    private final BeatStreamReader bsr;

    private final ArrayList<Beat> beats;

    public Track(String path, String mapId) throws IOException {
        this.path = path;

        Core.audioManager.load(path + "/track.ogg");
        map = MapConfigure.MapFromFolder(path);
        bsr = new BeatStreamReader(new File(path + "/" + mapId + ".to"));
        MapData mapData = map.Maps.get(mapId);
        beats = new ArrayList<>(mapData.ClickCount + mapData.SlideCount);
        beats.add(bsr.GetNextBeat());
    }

    public void start() throws IOException {
        Core.audioManager.play(path + "/track.ogg");
    }

    /**
     * Gets the serial id of this object.
     * 
     * @return The serial id of this object.
     */
    @Override
    public int getId() {
        return ID;
    }

    /**
     * Draws the current {@code Graphics g} to the screen.
     * 
     * @param g The {@code Graphics} to draw.
     */
    @Override
    public void draw(Graphics g) {
        g.setColor(Color.BLACK);
        g.fillRect(0, 0, Config.WIDTH, Config.HEIGHT);

        // draw grid
        g.setColor(Color.WHITE);
        for (int y = -100; y < Config.HEIGHT + 100; y += 100) {
            for (int x = -100; x < Config.WIDTH + 100; x += 100) {
                g.fillOval(
                    x + (int) (xOffset % 100), y + (int) (yOffset % 100), 2, 2
                );
            }
        }

        // draw FPS
        g.drawString(Core.pane.renderLoop.staticFps + " fps", 10, 20);

        // draw cursor
        g.fillOval((Config.WIDTH / 2) - 5, (Config.HEIGHT / 2) - 5, 10, 10);
    }

    /**
     * Mouse up event handle
     * 
     * @param e
     */
    @Override
    public void mouseUp(MouseEvent e) {}

    /**
     * Mouse down event handle
     * 
     * @param e
     */
    @Override
    public void mouseDown(MouseEvent e) {}

    /**
     * Key down event handle
     * 
     * @param e
     */
    @Override
    public void keyDown(KeyEvent e) {
        if (e.getKeyCode() == Config.UP_KEY) UP = true;
        else if (e.getKeyCode() == Config.DOWN_KEY) DOWN = true;
        else if (e.getKeyCode() == Config.LEFT_KEY) LEFT = true;
        else if (e.getKeyCode() == Config.RIGHT_KEY) RIGHT = true;
        else if (Config.TAP_KEYS.contains(e.getKeyCode())) {

        }
    }

    /**
     * Key up event handle
     * 
     * @param e
     */
    @Override
    public void keyUp(KeyEvent e) {
        if (e.getKeyCode() == Config.UP_KEY) UP = false;
        else if (e.getKeyCode() == Config.DOWN_KEY) DOWN = false;
        else if (e.getKeyCode() == Config.LEFT_KEY) LEFT = false;
        else if (e.getKeyCode() == Config.RIGHT_KEY) RIGHT = false;
    }

    @Override
    public void update(double delta) {
        yAccel *= glideFactor;
        xAccel *= glideFactor;

        if (UP) yAccel += speed;
        if (DOWN) yAccel -= speed;
        if (LEFT) xAccel += speed;
        if (RIGHT) xAccel -= speed;

        yOffset += yAccel * delta;
        xOffset += xAccel * delta;

        // load more beats
        long offset = Core.audioManager.getCurrent().getPlayingOffset()
            .asMilliseconds();
        Beat new_beat = beats.get(beats.size() - 1);
        while (new_beat.InitTimestamp < offset) {
            new_beat = bsr.GetNextBeat();
            beats.add(new_beat);
        }
    }
}

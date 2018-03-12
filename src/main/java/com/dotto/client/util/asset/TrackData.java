package com.dotto.client.util.asset;

/**
 * This class contains information about a BeatMap track.
 * 
 * @author SoraKatadzuma
 */
public final class TrackData {
    /** The name of this song. */
    public final String TrackName;
    /** The author of this song. */
    public final String TrackAuthor;
    /** The source this song came from, be it a game, show, or movie, etc. */
    public final String Source;
    /** The speed of this track. */
    public final byte bpm;
    /** The preview times for this track in milliseconds. */
    public final int[] Preview;

    /**
     * Constructs a new instance of the {@code Track} object with the given parameters.
     * 
     * @param TrackName The name of this track.
     * @param TrackAuthor The name of the author of this track.
     * @param bpm The speed of this track.
     * @param Source The origination of this track.
     * @param Preview The preview times for this track in milliseconds.
     */
    public TrackData(
        String TrackName, String TrackAuthor, String Source, byte bpm,
        int[] Preview
    ) {
        this.TrackName = TrackName;
        this.TrackAuthor = TrackAuthor;
        this.Source = Source;
        this.bpm = bpm;
        this.Preview = Preview;
    }
}
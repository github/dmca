package awais.instagrabber.utils;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.repositories.responses.Media;

public final class KeywordsFilterUtils {

    private final ArrayList<String> keywords;

    public KeywordsFilterUtils(final ArrayList<String> keywords){
        this.keywords = keywords;
    }

    public boolean filter(final String caption){
        if(caption == null) return false;
        if(keywords.isEmpty()) return false;
        final String temp = caption.toLowerCase();
        for(final String s:keywords){
            if(temp.contains(s)) return true;
        }
        return false;
    }

    public boolean filter(final Media media){
        if(media == null) return false;
        final Caption c = media.getCaption();
        if(c == null) return false;
        if(keywords.isEmpty()) return false;
        final String temp = c.getText().toLowerCase();
        for(final String s:keywords){
            if(temp.contains(s)) return true;
        }
        return false;
    }

    public List<Media> filter(final List<Media> media){
        if(keywords.isEmpty()) return media;
        if(media == null) return new ArrayList<>();

        final List<Media> result= new ArrayList<>();
        for(final Media m:media){
            if(!filter(m)) result.add(m);
        }
        return result;
    }
}

package awais.instagrabber.repositories.responses;

import java.util.Objects;

public class Place {
    private final Location location;
    // for search
    private final String title; // those are repeated within location
    private final String subtitle; // address
    private final String slug; // browser only; for end of address
    // for location info
    private final String status;

    public Place(final Location location,
                 final String title,
                 final String subtitle,
                 final String slug,
                 final String status) {
        this.location = location;
        this.title = title;
        this.subtitle = subtitle;
        this.slug = slug;
        this.status = status;
    }

    public Location getLocation() {
        return location;
    }

    public String getTitle() {
        return title;
    }

    public String getSubtitle() {
        return subtitle;
    }

    public String getSlug() {
        return slug;
    }

    public String getStatus() {
        return status;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Place place = (Place) o;
        return Objects.equals(location, place.location) &&
                Objects.equals(title, place.title) &&
                Objects.equals(subtitle, place.subtitle) &&
                Objects.equals(slug, place.slug) &&
                Objects.equals(status, place.status);
    }

    @Override
    public int hashCode() {
        return Objects.hash(location, title, subtitle, slug, status);
    }
}

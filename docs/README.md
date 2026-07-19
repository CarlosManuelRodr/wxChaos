# wxChaos website

This directory is the complete source for the wxChaos GitHub Pages site.
Configure **Settings → Pages → Deploy from a branch** to publish the `/docs`
folder from the desired branch.

The repository-level `README.md`, `README.es.md`, and `README.ja.md` remain
independent so they continue to render naturally on GitHub.

## Structure

- `_data/content.yml` contains the English, Spanish, and Japanese page copy.
- `_layouts/home.html` contains the shared landing-page structure.
- `assets/css/style.css` contains the complete responsive theme.
- `assets/js/site.js` provides the mobile menu and theme preference.
- `index.md`, `es/index.md`, and `ja/index.md` define the localized routes.

The site expects the project URL configured in `_config.yml`. Use Jekyll's
`relative_url` and `absolute_url` filters for new internal links so previews and
the `/wxChaos` project path continue to work correctly.

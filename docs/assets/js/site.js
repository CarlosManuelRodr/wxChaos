(function () {
  "use strict";

  var root = document.documentElement;
  var header = document.querySelector("[data-header]");
  var menuButton = document.querySelector(".menu-toggle");
  var navigation = document.querySelector("#site-nav");
  var themeButton = document.querySelector("[data-theme-toggle]");

  function updateHeader() {
    if (header) {
      header.classList.toggle("is-scrolled", window.scrollY > 18);
    }
  }

  function closeMenu() {
    if (!header || !menuButton) {
      return;
    }

    header.classList.remove("is-open");
    menuButton.setAttribute("aria-expanded", "false");
  }

  updateHeader();
  window.addEventListener("scroll", updateHeader, { passive: true });

  if (menuButton && header) {
    menuButton.addEventListener("click", function () {
      var willOpen = !header.classList.contains("is-open");
      header.classList.toggle("is-open", willOpen);
      menuButton.setAttribute("aria-expanded", String(willOpen));
    });
  }

  if (navigation) {
    navigation.addEventListener("click", function (event) {
      if (event.target.closest("a")) {
        closeMenu();
      }
    });
  }

  window.addEventListener("resize", function () {
    if (window.innerWidth > 900) {
      closeMenu();
    }
  });

  if (themeButton) {
    themeButton.addEventListener("click", function () {
      var nextTheme = root.dataset.theme === "dark" ? "light" : "dark";
      root.dataset.theme = nextTheme;

      try {
        localStorage.setItem("wxchaos-theme", nextTheme);
      } catch (error) {
        // The selected theme still applies for this page when storage is unavailable.
      }
    });
  }
})();

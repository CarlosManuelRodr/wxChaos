(function () {
  "use strict";

  var header = document.querySelector("[data-header]");
  var menuButton = document.querySelector(".menu-toggle");
  var navigation = document.querySelector("#site-nav");

  function closeMenu() {
    if (!header || !menuButton) {
      return;
    }

    header.classList.remove("is-open");
    menuButton.setAttribute("aria-expanded", "false");
  }

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
})();

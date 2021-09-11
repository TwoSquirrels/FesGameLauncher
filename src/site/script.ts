// types

declare const constant: {
    site: {
      name: string,
      message: string,
      url: string,
      libs: {
        css: string[],
        js: string[],
      },
      version: string,
    }, page: {
      keywords: string[],
      description: string,
      author: string,
      color: string,
      type: string,
      title: string,
      twitter: string,
      libs: {
        css: string[],
        js: string[],
      },
      path: string,
      top: string,
    },
  };
declare const extra: { path: string, top: () => string };

// utils

function maximizingAdjustment() {
  document.documentElement.style.setProperty("--width",  `${document.documentElement.clientWidth }px`);
  document.documentElement.style.setProperty("--height", `${document.documentElement.clientHeight}px`);
}

/*!
license header
*/

// references

/// <reference path="script.ts"/>

// contents

type Content = {
  id: string;
  title: string;
  version: string;
  description?: string;
  author?: string;
  added: number;
  icon?: string;
  detail?: string;
};
type Game = Content & {
  game: {
    file: {
      win32?: string;
      win64?: string;
      winarm?: string;
      osx64?: string;
      linux64?: string;
      linuxarm?: string;
      linuxarm64?: string;
      site?: string;
    };
    offline: boolean;
    difficulty: 0 | 1 | 2;
  };
};
type Movie = Content & {
  movie: {
    file: string;
  };
};
type Other = Content & {
  other: {
    site: string;
  };
};

declare const electron: {
  contents: {
    getGames: () => Promise<Game[]>;
    getMovies: () => Promise<Movie[]>;
    getOthers: () => Promise<Other[]>;
    launch: (
      id: string,
      platform: string,
      arguments?: string[]
    ) => Promise<unknown>;
  };
};

// define functions

function isElectron(): boolean {
  return typeof electron !== "undefined";
}

function changeExtra(newPath: string): void {
  history.replaceState("", "", `${extra.top()}launcher/${newPath}`);
  extra.path = newPath;
}

let tab: string = "games";
let item: string | null = null;
function switchTab(tabName: string) {
  tab = tabName;
  item = null;
  changeExtra(`${tab}`);
}

function switchItem(itemName: string) {
  item = itemName;
  changeExtra(`${tab}/${item}`);
}

// register events

window.onload = event => {
  maximizingAdjustment();
  if (isElectron())
    electron.contents.getGames().then(console.log).catch(console.error);
};

window.onresize = event => {
  maximizingAdjustment();
};

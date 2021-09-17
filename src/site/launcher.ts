/*!
license header
*/

// references

/// <reference path="script.ts"/>

// define functions

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
};

window.onresize = event => {
  maximizingAdjustment();
};

async function init() {

  // window

  await nw.Window.open("/build/nw/index.html", {}, function (win) {
  });
  const win = nw.Window.get();

  // events

  win.on("close", () => (async () => {
    win.hide();
    global.alert("The close button has been pressed.");
    win.close(true);
    await onStop();
  })());

  global.alert("FesGameLauncher has been launched!");

}

async function onStop() {
  console.log("FesGameLauncher is being terminated...");
}

init();

(async () => {
  await new Promise<void>(resolve => setTimeout(() => {
    console.log("World!");
    resolve();
  }));
  console.log("Hello,");
})();

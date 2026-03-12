{
  async function downloadCLinks() {
    // 1. Request permission to a local folder
    let dirHandle;
    try {
      dirHandle = await window.showDirectoryPicker();
    } catch (e) {
      console.error("Folder selection cancelled.");
      return;
    }

    // 2. Find all links ending in .c
    const links = Array.from(document.querySelectorAll("a")).filter(
      (a) => a.innerText.trim().endsWith(".c") || a.href.endsWith(".c"),
    );

    console.log(`Found ${links.length} files. Starting download...`);

    for (const link of links) {
      const fileName = link.innerText.trim();
      const url = link.href;

      try {
        console.log(`Fetching: ${fileName}...`);

        // 3. Fetch the content of the .c file
        const response = await fetch(url);
        const text = await response.text();

        // 4. Create the file in your Windows folder
        const fileHandle = await dirHandle.getFileHandle(fileName, {
          create: true,
        });
        const writable = await fileHandle.createWritable();
        await writable.write(text);
        await writable.close();

        console.log(`✅ Saved: ${fileName}`);
      } catch (err) {
        console.error(`❌ Failed to save ${fileName}:`, err);
      }
    }

    alert("Finished syncing OSTEP C files!");
  }

  // Start the process
  downloadCLinks();
}

{
  async function syncOSTEPFiles() {
    // Check browser support
    if (!window.showDirectoryPicker) {
      alert("Please use Chrome or Edge for the File System Access API.");
      return;
    }

    // Helper to process a UL element within a specific local folder handle
    async function processList(ulElement, currentHandle, pathString) {
      const items = ulElement.querySelectorAll(":scope > li");

      for (const li of items) {
        // 1. Check if this LI represents a directory (contains a UL)
        const subUl = li.querySelector("ul");
        const folderNameMatch = li.firstChild?.textContent.match(/^([\w-]+)\//);

        if (subUl && folderNameMatch) {
          const folderName = folderNameMatch[1];
          const newPath = pathString
            ? `${pathString} > ${folderName}`
            : folderName;

          console.log(`📂 Entering Directory: ${newPath}`);
          alert(`Next Step: Prepare folder for [ ${newPath} ]`);

          try {
            // Ask user to create/select the matching local folder
            const subHandle = await currentHandle.getDirectoryHandle(
              folderName,
              { create: true },
            );
            await processList(subUl, subHandle, newPath);
          } catch (err) {
            console.error(
              `User skipped or failed to create folder: ${folderName}`,
              err,
            );
          }
        }

        // 2. Check for target files (.c, .h, .d)
        const links = li.querySelectorAll(":scope > a");
        for (const link of links) {
          const fileName = link.innerText.trim();
          const isTargetFile = /\.(c|h|d)$/.test(fileName);

          if (isTargetFile) {
            try {
              console.log(`📄 Fetching: ${fileName}...`);
              const response = await fetch(link.href);
              const text = await response.text();

              const fileHandle = await currentHandle.getFileHandle(fileName, {
                create: true,
              });
              const writable = await fileHandle.createWritable();
              await writable.write(text);
              await writable.close();
              console.log(`✅ Saved ${fileName} to ${pathString}`);
            } catch (err) {
              console.error(`❌ Error saving ${fileName}:`, err);
            }
          }
        }
      }
    }

    // Initialize the root
    try {
      alert(
        "Select the ROOT folder where you want the 'code' structure to begin.",
      );
      const rootHandle = await window.showDirectoryPicker();
      const startUl = document.querySelector("ul.flushlist > ul");

      if (!startUl) {
        alert("Could not find the starting <ul>. Please check the selector.");
        return;
      }

      await processList(startUl, rootHandle, "");
      alert("🎉 Structure sync complete!");
    } catch (e) {
      console.log("Sync cancelled.");
    }
  }

  syncOSTEPFiles();
}

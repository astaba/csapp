(function () {
    const VIDEO_DIR = '/video/';
    const EXT_RE = /\.(mp4|webm|ogg|m4v|mov)$/i;

    function buildUrl(name) {
        return VIDEO_DIR + name;
    }

    function createTile(filename) {
        const fileUrl = buildUrl(filename);

        const tile = document.createElement('button');
        tile.type = 'button';
        tile.className = 'tile';
        tile.title = filename;

        const preview = document.createElement('video');
        preview.src = fileUrl;
        preview.preload = 'metadata';
        preview.muted = true;
        preview.playsInline = true;

        const meta = document.createElement('div');
        meta.className = 'meta';

        const title = document.createElement('div');
        title.className = 'title';
        title.textContent = filename;

        const sub = document.createElement('div');
        sub.className = 'sub';
        sub.textContent = filename.split('.').pop().toLowerCase();

        meta.appendChild(title);
        meta.appendChild(sub);

        tile.appendChild(preview);
        tile.appendChild(meta);

        tile.addEventListener('mouseenter', () => preview.play().catch(() => {}));
        tile.addEventListener('mouseleave', () => {
            preview.pause();
            preview.currentTime = 0;
        });

        tile.addEventListener('click', () => loadIntoPlayer(fileUrl));

        return tile;
    }

    async function getVideoFiles() {
        try {
            const res = await fetch(VIDEO_DIR + 'index.json', { cache: 'no-cache' });
            if (!res.ok) return [];

            const json = await res.json();
            if (!Array.isArray(json)) return [];

            return json.filter(name => EXT_RE.test(name));
        } catch {
            return [];
        }
    }

    function loadIntoPlayer(url) {
        const player = document.getElementById('player');
        const source = document.getElementById('player-source');
        const download = document.getElementById('download');
        const resEl = document.getElementById('res');

        if (!player || !source) return;

        source.src = url;
        source.type = guessMime(url);

        player.load();

        player.addEventListener('loadedmetadata', () => {
            const w = player.videoWidth;
            const h = player.videoHeight;
            resEl.textContent = (w && h) ? `${w}×${h}` : 'auto';
        }, { once: true });

        download.href = url;

        player.play().catch(() => {});
    }

    function guessMime(url) {
        const ext = (url.split('.').pop() || '').toLowerCase();

        switch (ext) {
            case 'mp4': return 'video/mp4';
            case 'webm': return 'video/webm';
            case 'ogg': return 'video/ogg';
            case 'm4v': return 'video/x-m4v';
            case 'mov': return 'video/quicktime';
            default: return '';
        }
    }

    document.addEventListener('DOMContentLoaded', async () => {
        const gallery = document.getElementById('gallery');
        const empty = document.getElementById('gallery-empty');

        const files = await getVideoFiles();

        if (!files.length) {
            empty.style.display = '';
            return;
        }

        empty.style.display = 'none';

        files.forEach(name => {
            gallery.appendChild(createTile(name));
        });

        // load first video
        loadIntoPlayer(buildUrl(files[0]));
    });
})();

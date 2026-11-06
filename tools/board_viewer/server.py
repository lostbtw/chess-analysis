"""Local web server for the board viewer.

Serves index.html and answers /api/eval?fen=... by running the engine's `eval` command.
Run from anywhere:  python tools/board_viewer/server.py
Then open:          http://localhost:8000
"""

import json
import subprocess
import sys
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from urllib.parse import parse_qs, urlparse

PAGE_DIRECTORY = Path(__file__).resolve().parent
PROJECT_ROOT = PAGE_DIRECTORY.parents[1]
ENGINE_PATH = PROJECT_ROOT / "build" / ("chess.exe" if sys.platform == "win32" else "chess")
HOST = "127.0.0.1"
PORT = 8000


class BoardViewerHandler(SimpleHTTPRequestHandler):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(PAGE_DIRECTORY), **kwargs)

    def do_GET(self):
        url = urlparse(self.path)
        if url.path == "/api/eval":
            fen = parse_qs(url.query).get("fen", [""])[0]
            self.send_engine_result(["eval", fen] if fen else ["eval"])
        else:
            super().do_GET()

    def send_engine_result(self, arguments):
        result = subprocess.run([str(ENGINE_PATH), *arguments], capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            status, body = 200, result.stdout
        else:
            status, body = 400, json.dumps({"error": result.stderr.strip() or "Engine failed"})

        encoded_body = body.encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(encoded_body)))
        self.end_headers()
        self.wfile.write(encoded_body)


def main():
    if not ENGINE_PATH.exists():
        sys.exit(f"Engine not found at {ENGINE_PATH}. Build it first: cmake --build build")

    server = ThreadingHTTPServer((HOST, PORT), BoardViewerHandler)
    print(f"Board viewer: http://localhost:{PORT}  (Ctrl+C to stop)")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass


if __name__ == "__main__":
    main()

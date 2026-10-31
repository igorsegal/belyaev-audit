from pathlib import Path
from mcp.server.fastmcp import FastMCP
ROOT = Path(__file__).resolve().parent.parent
server = FastMCP("Belyaev Audit")
def safe_path(name: str) -> Path:
    path = (ROOT / name).resolve()
    if not path.is_relative_to(ROOT):
        raise ValueError("Access outside project denied")
    if ".git" in path.relative_to(ROOT).parts:
        raise ValueError("Git metadata access denied")
    return path
@server.tool()
def list_files() -> list[str]:
    return sorted(
        str(p.relative_to(ROOT))
        for p in ROOT.rglob("*")
        if p.is_file()
        and ".git" not in p.relative_to(ROOT).parts
        and "build" not in p.relative_to(ROOT).parts
    )
@server.tool()
def read_file(path: str) -> str:
    p = safe_path(path)
    if not p.is_file():
        raise ValueError("File not found")
    return p.read_text(encoding="utf-8-sig")
if __name__ == "__main__":
    server.run()

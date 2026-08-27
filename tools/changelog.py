#!/usr/bin/env python3
"""Секция CHANGELOG.md для одной версии.

Один источник для двух потребителей: тела релиза на GitHub и поля `notes` в
манифесте, откуда его читает страница обновления устройства. Отдельным
скриптом, а не строкой внутри workflow, чтобы то же извлечение можно было
проверить локально -- и чтобы забытая секция ловилась до сборки.
"""

from __future__ import annotations

import pathlib
import re
import sys

CHANGELOG = pathlib.Path(__file__).resolve().parents[1] / "CHANGELOG.md"


def section(version: str, text: str | None = None) -> str | None:
    """Текст секции `## <version>` без заголовка, или None, если её нет."""
    if text is None:
        text = CHANGELOG.read_text(encoding="utf-8")
    match = re.search(
        r"^## %s[ \t]*$(.*?)(?=^## |\Z)" % re.escape(version),
        text,
        re.M | re.S,
    )
    return match.group(1).strip() if match else None


def main() -> int:
    if len(sys.argv) != 2:
        print("использование: changelog.py <версия>", file=sys.stderr)
        return 2
    version = sys.argv[1]
    found = section(version)
    if found is None:
        print(
            f"Ошибка: в {CHANGELOG.name} нет секции '## {version}'. "
            "Добавьте её до выпуска релиза.",
            file=sys.stderr,
        )
        return 1
    print(found)
    return 0


if __name__ == "__main__":
    sys.exit(main())

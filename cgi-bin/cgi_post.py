#!/usr/bin/env python3
import os
import sys
from urllib.parse import parse_qs

method = os.environ.get("REQUEST_METHOD", "GET").upper()

STYLE = """
    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    :root {
      --bg:      #060a06;
      --surface: #0c130c;
      --border:  #1a2e1a;
      --green:   #39ff6a;
      --dim:     #2a4a2a;
      --muted:   #4a7a4a;
      --text:    #c8e6c8;
    }

    body {
      background: var(--bg);
      color: var(--text);
      font-family: 'JetBrains Mono', monospace;
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      padding: 2rem;
      overflow: hidden;
    }

    body::before {
      content: '';
      position: fixed;
      inset: 0;
      background-image:
        linear-gradient(var(--border) 1px, transparent 1px),
        linear-gradient(90deg, var(--border) 1px, transparent 1px);
      background-size: 40px 40px;
      opacity: .35;
      pointer-events: none;
    }

    body::after {
      content: '';
      position: fixed;
      left: 0; right: 0;
      height: 2px;
      background: linear-gradient(90deg, transparent, var(--green), transparent);
      animation: scan 4s linear infinite;
      opacity: .4;
      pointer-events: none;
    }
    @keyframes scan { from { top: -2px; } to { top: 100vh; } }

    .terminal {
      position: relative;
      width: 100%;
      max-width: 560px;
      background: var(--surface);
      border: 1px solid var(--dim);
      border-radius: 4px;
      box-shadow: 0 0 60px rgba(57,255,106,.07), 0 0 0 1px var(--border);
      animation: fadein .5s ease;
    }
    @keyframes fadein { from { opacity:0; transform:translateY(12px); } to { opacity:1; transform:none; } }

    .titlebar {
      display: flex;
      align-items: center;
      gap: .6rem;
      padding: .75rem 1rem;
      border-bottom: 1px solid var(--border);
    }
    .dot { width: 10px; height: 10px; border-radius: 50%; }
    .dot.r { background: #ff5f57; }
    .dot.y { background: #febc2e; }
    .dot.g { background: #28c840; }
    .title-text {
      margin-left: .5rem;
      font-size: .7rem;
      color: var(--muted);
      letter-spacing: .1em;
      text-transform: uppercase;
    }

    .body { padding: 1.75rem 2rem 2rem; }

    .prompt {
      font-size: .72rem;
      color: var(--muted);
      margin-bottom: 1.5rem;
      letter-spacing: .05em;
    }
    .prompt span { color: var(--green); }

    /* --- Formulaire --- */
    .field { margin-bottom: 1.25rem; }
    label {
      display: block;
      font-size: .65rem;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: .1em;
      margin-bottom: .4rem;
    }
    input[type=text] {
      width: 100%;
      background: var(--bg);
      border: 1px solid var(--dim);
      border-radius: 2px;
      color: var(--green);
      font-family: 'JetBrains Mono', monospace;
      font-size: .95rem;
      padding: .6rem .8rem;
      outline: none;
      transition: border-color .2s, box-shadow .2s;
      caret-color: var(--green);
    }
    input[type=text]:focus {
      border-color: var(--green);
      box-shadow: 0 0 0 2px rgba(57,255,106,.1);
    }
    input[type=text]::placeholder { color: var(--dim); }

    button {
      width: 100%;
      margin-top: .5rem;
      background: transparent;
      border: 1px solid var(--green);
      border-radius: 2px;
      color: var(--green);
      font-family: 'JetBrains Mono', monospace;
      font-size: .85rem;
      letter-spacing: .1em;
      text-transform: uppercase;
      padding: .7rem;
      cursor: pointer;
      transition: background .2s, box-shadow .2s;
    }
    button:hover {
      background: rgba(57,255,106,.08);
      box-shadow: 0 0 20px rgba(57,255,106,.15);
    }

    /* --- Résultat --- */
    .output-line {
      display: flex;
      align-items: baseline;
      gap: 1rem;
      padding: .55rem 0;
      border-bottom: 1px solid var(--border);
      animation: linein .3s ease both;
    }
    .output-line:last-of-type { border-bottom: none; }
    @keyframes linein { from { opacity:0; transform:translateX(-6px); } to { opacity:1; transform:none; } }
    .output-line:nth-child(1) { animation-delay: .1s; }
    .output-line:nth-child(2) { animation-delay: .2s; }

    .key {
      font-size: .7rem;
      color: var(--muted);
      min-width: 90px;
      text-transform: uppercase;
      letter-spacing: .08em;
    }
    .val {
      font-size: 1.05rem;
      color: var(--green);
      font-weight: 700;
      text-shadow: 0 0 12px rgba(57,255,106,.5);
    }

    .back {
      display: inline-block;
      margin-top: 1.5rem;
      font-size: .7rem;
      color: var(--muted);
      text-decoration: none;
      letter-spacing: .06em;
      transition: color .2s;
    }
    .back:hover { color: var(--green); }

    .footer {
      margin-top: 2rem;
      font-size: .65rem;
      color: var(--dim);
      letter-spacing: .06em;
    }
    .cursor {
      display: inline-block;
      width: 8px; height: 1em;
      background: var(--green);
      animation: blink .8s step-end infinite;
      vertical-align: text-bottom;
      margin-left: 2px;
    }
    @keyframes blink { 50% { opacity: 0; } }
"""

if method == "POST":
    content_length = int(os.environ.get("CONTENT_LENGTH", 0))
    raw_body = sys.stdin.read(content_length) if content_length > 0 else ""
    params = parse_qs(raw_body)

    name     = params.get("name",     [""])[0]
    message = params.get("message", [""])[0]

    body = f"""<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>CGI // POST</title>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700&display=swap" rel="stylesheet">
  <style>{STYLE}</style>
</head>
<body>
  <div class="terminal">
    <div class="titlebar">
      <div class="dot r"></div>
      <div class="dot y"></div>
      <div class="dot g"></div>
      <span class="title-text">cgi_post.py — stdout</span>
    </div>
    <div class="body">
      <div class="prompt">
        <span>POST</span> /cgi-bin/cgi_post.py
      </div>
      <div class="output-line">
        <span class="key">name</span>
        <span class="val">{name if name else "—"}</span>
      </div>
      <div class="output-line">
        <span class="key">message</span>
        <span class="val">{message if message else "—"}</span>
      </div>
      <a class="back" href="/cgi-bin/cgi_post.py">← back to form </a>
      <div class="footer">200 OK &nbsp;·&nbsp; method: POST &nbsp;·&nbsp; body parsed<span class="cursor"></span></div>
    </div>
  </div>
</body>
</html>"""

else:
    body = f"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>CGI // POST</title>
  <link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;700&display=swap" rel="stylesheet">
  <style>{STYLE}</style>
</head>
<body>
  <div class="terminal">
    <div class="titlebar">
      <div class="dot r"></div>
      <div class="dot y"></div>
      <div class="dot g"></div>
      <span class="title-text">cgi_post.py — stdin</span>
    </div>
    <div class="body">
      <div class="prompt">
        <span>GET</span> /cgi-bin/cgi_post.py &nbsp;· awaiting POST
      </div>
      <form method="POST" action="/cgi-bin/cgi_post.py">
        <div class="field">
          <label for="name">name</label>
          <input type="text" id="name" name="name" placeholder="Mi" required>
        </div>
        <div class="field">
          <label for="message">message</label>
          <input type="text" id="message" name="message" placeholder="Thanks for the work Claude !" required>
        </div>
        <button type="submit">&#9658; Send</button>
      </form>
    </div>
  </div>
</body>
</html>"""

print("Content-Type: text/html; charset=utf-8")
print(f"Content-Length: {len(body.encode('utf-8'))}")
print()
print(body)

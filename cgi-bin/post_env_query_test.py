#!/usr/bin/env python3
import os
import sys

print("Content-Type: text/html")
print("")  # blank line mandatory

print("<html><body>")
print("<h2>POST CGI test</h2>")

# --- Read the body from stdin ---
# For POST, your server must:
#   1. set CONTENT_LENGTH to the body size in bytes
#   2. pipe the request body into the script's stdin
# The script reads exactly CONTENT_LENGTH bytes from stdin

print("<h3>Request body (stdin)</h3>")
content_length = os.environ.get("CONTENT_LENGTH", "")
if content_length and int(content_length) > 0:
    body = sys.stdin.read(int(content_length))
    print(f"<p>Read <b>{len(body)}</b> bytes from stdin</p>")
    print(f"<p>Raw body: <code>{body}</code></p>")

    # If Content-Type is application/x-www-form-urlencoded, parse it
    ct = os.environ.get("CONTENT_TYPE", "")
    if "x-www-form-urlencoded" in ct:
        print("<h4>Parsed form fields</h4>")
        print("<ul>")
        for pair in body.split("&"):
            if "=" in pair:
                key, val = pair.split("=", 1)
                # URL decode the + signs (spaces in form data)
                key = key.replace("+", " ")
                val = val.replace("+", " ")
                print(f"<li><b>{key}</b> = {val}</li>")
        print("</ul>")
    elif "application/json" in ct:
        print("<h4>JSON body received</h4>")
        print(f"<pre>{body}</pre>")
else:
    print("<p style='color:red'>No body received — CONTENT_LENGTH is missing or 0</p>")
    print("<p>This means your server is not piping the request body to stdin, "
          "or not setting CONTENT_LENGTH correctly</p>")

# --- Mandatory env vars for POST ---
print("<h3>Mandatory POST env vars</h3>")
REQUIRED = [
    "REQUEST_METHOD",    # must be POST
    "CONTENT_LENGTH",    # must match actual body size
    "CONTENT_TYPE",      # must match the request Content-Type header
    "QUERY_STRING",      # empty string if no query string, but must exist
    "SERVER_NAME",
    "SERVER_PORT",
    "SCRIPT_FILENAME",
]

print("<table border='1' cellpadding='6'>")
print("<tr><th>Variable</th><th>Value</th><th>Status</th></tr>")
for var in REQUIRED:
    val = os.environ.get(var)
    if val is not None:
        status = "<span style='color:green'>OK</span>"
        display = val if val else "<i>(empty string)</i>"
    else:
        status = "<span style='color:red'>MISSING</span>"
        display = ""
    print(f"<tr><td><code>{var}</code></td><td><code>{display}</code></td><td>{status}</td></tr>")
print("</table>")

# --- Specific checks ---
print("<h3>Sanity checks</h3>")
print("<ul>")

method = os.environ.get("REQUEST_METHOD", "")
if method == "POST":
    print("<li style='color:green'>REQUEST_METHOD is POST — correct</li>")
else:
    print(f"<li style='color:red'>REQUEST_METHOD is '{method}' — should be POST</li>")

cl = os.environ.get("CONTENT_LENGTH", "")
if cl and int(cl) > 0:
    print(f"<li style='color:green'>CONTENT_LENGTH = {cl} — correct</li>")
else:
    print("<li style='color:red'>CONTENT_LENGTH missing or zero — server bug</li>")

ct = os.environ.get("CONTENT_TYPE", "")
if ct:
    print(f"<li style='color:green'>CONTENT_TYPE = {ct}</li>")
else:
    print("<li style='color:red'>CONTENT_TYPE missing — server bug</li>")

print("</ul>")

# --- All env vars ---
print("<h3>All environment variables</h3>")
print("<table border='1' cellpadding='6'>")
print("<tr><th>Variable</th><th>Value</th></tr>")
for key, val in sorted(os.environ.items()):
    print(f"<tr><td><code>{key}</code></td><td><code>{val}</code></td></tr>")
print("</table>")

# --- argv ---
print("<h3>argv</h3>")
if len(sys.argv) > 1:
    for i, arg in enumerate(sys.argv):
        print(f"<p>argv[{i}] = <code>{arg}</code></p>")
else:
    print("<p>No command line arguments (normal)</p>")

print("</body></html>")

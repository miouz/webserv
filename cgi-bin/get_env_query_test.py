#!/usr/bin/env python3
import os
import sys

# CGI scripts must print the full HTTP response to stdout
# Your server reads this stdout and forwards it to the client

print("Content-Type: text/html")
print("")  # blank line = end of headers, mandatory

print("<html><body>")
print("<h2>GET CGI test</h2>")

# --- Query string ---
# Your server must set QUERY_STRING to everything after the '?'
# e.g. /cgi-bin/get_test.py?name=alice&age=30
# → QUERY_STRING=name=alice&age=30

print("<h3>QUERY_STRING</h3>")
qs = os.environ.get("QUERY_STRING", "")
if qs:
    print(f"<p>Raw: <code>{qs}</code></p>")
    pairs = qs.split("&")
    print("<ul>")
    for pair in pairs:
        if "=" in pair:
            key, val = pair.split("=", 1)
            print(f"<li><b>{key}</b> = {val}</li>")
    print("</ul>")
else:
    print("<p style='color:red'>QUERY_STRING is empty — did you add ?key=value to the URL?</p>")

# --- Mandatory env vars your server must always set ---
print("<h3>Mandatory env vars</h3>")
REQUIRED = [
    "REQUEST_METHOD",
    "QUERY_STRING",
    "SERVER_NAME",
    "SERVER_PORT",
    "SCRIPT_FILENAME",
    "PATH_INFO",
]

print("<table border='1' cellpadding='6'>")
print("<tr><th>Variable</th><th>Value</th><th>Status</th></tr>")
for var in REQUIRED:
    val = os.environ.get(var)
    if val is not None:
        status = "<span style='color:green'>OK</span>"
    else:
        status = "<span style='color:red'>MISSING</span>"
        val = ""
    print(f"<tr><td><code>{var}</code></td><td><code>{val}</code></td><td>{status}</td></tr>")
print("</table>")

# --- All env vars (so you can see everything your server passes) ---
print("<h3>All environment variables</h3>")
print("<table border='1' cellpadding='6'>")
print("<tr><th>Variable</th><th>Value</th></tr>")
for key, val in sorted(os.environ.items()):
    print(f"<tr><td><code>{key}</code></td><td><code>{val}</code></td></tr>")
print("</table>")

# --- Command line arguments ---
# CGI spec allows args to be passed via argv for indexed queries
# Most modern CGI doesn't use this but your server may set it
print("<h3>argv (command line arguments)</h3>")
if len(sys.argv) > 1:
    print("<ul>")
    for i, arg in enumerate(sys.argv):
        print(f"<li>argv[{i}] = <code>{arg}</code></li>")
    print("</ul>")
else:
    print("<p>No command line arguments (this is normal for standard CGI)</p>")

print("</body></html>")

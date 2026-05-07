#!/usr/bin/env python3
import os
import uuid

cookie = os.environ.get("HTTP_COOKIE", "")

# Parse existing session_id from cookie string if present
session_id = None
for part in cookie.split(";"):
    part = part.strip()
    if part.startswith("session_id="):
        session_id = part.split("=", 1)[1]

if session_id:
    # Returning visitor
    body = f"<h1>Welcome back! Session: {session_id}</h1>"
    print("Content-Type: text/html")
    print()  # blank line separates headers from body
    print(body)
else:
    # New visitor — generate session ID and set cookie
    new_id = str(uuid.uuid4())
    body = f"<h1>New session created: {new_id}</h1>"
    print("Content-Type: text/html")
    print(f"Set-Cookie: session_id={new_id}; Path=/; HttpOnly")
    print()  # blank line
    print(body)

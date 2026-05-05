#!/usr/bin/env python3
"""
Mini CGI — méthode POST
Lancer avec : python3 -m http.server --cgi 8080
Accéder via  : http://localhost:8080/cgi-bin/cgi_post.py
"""

import os

print("Content-Type: text/html; charset=utf-8")
print()

method = os.environ.get("REQUEST_METHOD", "GET").upper()

# Styles communs
style = """
  <style>
    body { font-family: monospace; background: #0d1117; color: #c9d1d9;
           display: flex; justify-content: center; padding: 4rem; }
    .card { background: #161b22; border: 1px solid #30363d;
            border-radius: 8px; padding: 2rem; max-width: 480px; width: 100%; }
    h1   { color: #3fb950; margin-top: 0; }
    .badge { display: inline-block; background: #238636;
             color: #fff; font-size: .75rem; padding: .2rem .6rem;
             border-radius: 20px; margin-bottom: 1.5rem; }
    label { display: block; margin-bottom: .3rem; color: #8b949e; font-size: .85rem; }
    input[type=text], input[type=number] {
      width: 100%; box-sizing: border-box;
      background: #0d1117; border: 1px solid #30363d;
      color: #c9d1d9; padding: .5rem .75rem;
      border-radius: 6px; font-family: monospace; font-size: 1rem;
      margin-bottom: 1rem; }
    button { background: #238636; color: #fff; border: none;
             padding: .6rem 1.4rem; border-radius: 6px;
             font-family: monospace; font-size: 1rem; cursor: pointer; }
    button:hover { background: #2ea043; }
    table { border-collapse: collapse; width: 100%; margin-top: 1rem; }
    td    { padding: .5rem .75rem; border-bottom: 1px solid #21262d; }
    td:first-child { color: #8b949e; width: 40%; }
    .back { display: inline-block; margin-top: 1.5rem;
            color: #58a6ff; text-decoration: none; font-size: .85rem; }
  </style>
"""

if method == "POST":
    # --- Traitement du formulaire POST ---
    form = cgi.FieldStorage()
    nom     = form.getvalue("nom", "").strip()
    message = form.getvalue("message", "").strip()

    nom_s     = cgi.escape(nom)     if nom     else "<em>vide</em>"
    message_s = cgi.escape(message) if message else "<em>vide</em>"

    print(f"""<!DOCTYPE html>
<html lang="fr">
<head><meta charset="UTF-8"><title>CGI — POST (résultat)</title>{style}</head>
<body>
  <div class="card">
    <span class="badge">POST</span>
    <h1>Formulaire reçu ✓</h1>
    <table>
      <tr><td>Champ <code>nom</code></td><td>{nom_s}</td></tr>
      <tr><td>Champ <code>message</code></td><td>{message_s}</td></tr>
    </table>
    <a class="back" href="/cgi-bin/cgi_post.py">← Retour au formulaire</a>
  </div>
</body>
</html>""")

else:
    # --- Affichage du formulaire (GET initial) ---
    print(f"""<!DOCTYPE html>
<html lang="fr">
<head><meta charset="UTF-8"><title>CGI — POST (formulaire)</title>{style}</head>
<body>
  <div class="card">
    <span class="badge">POST</span>
    <h1>Envoyer un message</h1>
    <form method="POST" action="/cgi-bin/cgi_post.py">
      <label for="nom">Votre nom</label>
      <input type="text" id="nom" name="nom" placeholder="Alice" required>

      <label for="message">Votre message</label>
      <input type="text" id="message" name="message" placeholder="Bonjour le monde !" required>

      <button type="submit">Envoyer →</button>
    </form>
  </div>
</body>
</html>""")

#!/usr/bin/env php-cgi
<?php
// ============================================================
//  webserv CGI tester — 42 project
//  Tests: env vars, GET/POST params, headers, file upload,
//         cookies, chunked body, redirects, and more.
// ============================================================

// ── Helpers ──────────────────────────────────────────────────

function h(string $s): string {
    return htmlspecialchars($s, ENT_QUOTES | ENT_SUBSTITUTE, 'UTF-8');
}

function env(string $key): string {
    return isset($_SERVER[$key]) ? h($_SERVER[$key]) : '<span class="nil">—</span>';
}

function section(string $title, string $body): string {
    return <<<HTML
    <section>
      <h2>{$title}</h2>
      {$body}
    </section>
    HTML;
}

function table(array $rows): string {
    if (empty($rows)) {
        return '<p class="nil">( empty )</p>';
    }
    $html = '<table><thead><tr><th>Key</th><th>Value</th></tr></thead><tbody>';
    foreach ($rows as $k => $v) {
        $html .= '<tr><td>' . h((string)$k) . '</td><td>' . h((string)$v) . '</td></tr>';
    }
    return $html . '</tbody></table>';
}

function badge(string $label, string $color = '#4ade80'): string {
    return "<span class='badge' style='background:{$color}'>{$label}</span>";
}

// ── Handle actions ───────────────────────────────────────────

$action  = $_GET['action'] ?? '';
$method  = $_SERVER['REQUEST_METHOD'] ?? 'GET';

// -- 1. Set / read a cookie
if ($action === 'set_cookie') {
    header('Set-Cookie: webserv_test=hello42; Path=/; HttpOnly');
    header('Location: ' . strtok($_SERVER['REQUEST_URI'] ?? '/', '?') . '?action=cookie_set');
    exit;
}

// -- 2. Redirect test
if ($action === 'redirect') {
    header('HTTP/1.1 302 Found');
    header('Location: ' . strtok($_SERVER['REQUEST_URI'] ?? '/', '?') . '?action=after_redirect');
    exit;
}

// -- 3. Custom status
if ($action === 'teapot') {
    header('HTTP/1.1 418 I\'m a teapot');
}

// -- 4. Large body response
if ($action === 'large_body') {
    header('Content-Type: text/plain');
    $chunk = str_repeat("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n", 100);
    for ($i = 0; $i < 10; $i++) {
        echo $chunk;
    }
    exit;
}

// -- 5. Raw POST body echo
if ($action === 'echo_body' && $method === 'POST') {
    header('Content-Type: text/plain');
    echo file_get_contents('php://input');
    exit;
}

// ── Start HTML output ────────────────────────────────────────
header('Content-Type: text/html; charset=UTF-8');
?>
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>webserv CGI tester</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@400;700&family=Space+Grotesk:wght@400;600;700&display=swap');

  :root {
    --bg:      #0d0f14;
    --surface: #161b24;
    --border:  #252d3d;
    --accent:  #00e5ff;
    --accent2: #ff4081;
    --ok:      #4ade80;
    --warn:    #fbbf24;
    --text:    #cdd6f4;
    --muted:   #6272a4;
    --mono:    'JetBrains Mono', monospace;
    --sans:    'Space Grotesk', sans-serif;
  }

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    background: var(--bg);
    color: var(--text);
    font-family: var(--sans);
    font-size: 15px;
    line-height: 1.6;
    padding: 2rem 1rem 4rem;
  }

  header {
    text-align: center;
    margin-bottom: 2.5rem;
  }

  header h1 {
    font-size: 2.2rem;
    font-weight: 700;
    letter-spacing: -0.03em;
    color: var(--accent);
    text-shadow: 0 0 24px rgba(0,229,255,.35);
  }

  header p {
    color: var(--muted);
    font-family: var(--mono);
    font-size: 0.85rem;
    margin-top: .4rem;
  }

  .pill {
    display: inline-block;
    padding: .2em .7em;
    border-radius: 999px;
    font-family: var(--mono);
    font-size: .78rem;
    font-weight: 700;
    background: var(--accent);
    color: #000;
    margin-left: .5rem;
  }

  main {
    max-width: 980px;
    margin: 0 auto;
    display: grid;
    gap: 1.5rem;
  }

  section {
    background: var(--surface);
    border: 1px solid var(--border);
    border-radius: 12px;
    padding: 1.4rem 1.6rem;
  }

  h2 {
    font-size: 1rem;
    font-weight: 700;
    text-transform: uppercase;
    letter-spacing: .08em;
    color: var(--accent);
    margin-bottom: 1rem;
    display: flex;
    align-items: center;
    gap: .5rem;
  }

  table {
    width: 100%;
    border-collapse: collapse;
    font-family: var(--mono);
    font-size: .82rem;
  }

  th {
    text-align: left;
    color: var(--muted);
    font-weight: 700;
    padding: .4rem .6rem;
    border-bottom: 1px solid var(--border);
  }

  td {
    padding: .4rem .6rem;
    border-bottom: 1px solid var(--border);
    word-break: break-all;
  }

  tr:last-child td { border-bottom: none; }
  tr:hover td { background: rgba(0,229,255,.04); }

  .nil { color: var(--muted); font-family: var(--mono); font-size: .85rem; }

  .badge {
    display: inline-block;
    padding: .15em .55em;
    border-radius: 6px;
    font-size: .78rem;
    font-weight: 700;
    color: #000;
    margin-right: .3rem;
  }

  /* Action buttons */
  .actions {
    display: flex;
    flex-wrap: wrap;
    gap: .7rem;
    margin-top: .5rem;
  }

  a.btn, button.btn {
    display: inline-flex;
    align-items: center;
    gap: .4rem;
    padding: .5em 1.1em;
    border-radius: 8px;
    font-family: var(--sans);
    font-size: .88rem;
    font-weight: 600;
    cursor: pointer;
    text-decoration: none;
    border: 1.5px solid transparent;
    transition: filter .15s, transform .1s;
  }

  a.btn:hover, button.btn:hover { filter: brightness(1.15); transform: translateY(-1px); }

  .btn-cyan   { background: var(--accent);  color: #000; }
  .btn-pink   { background: var(--accent2); color: #fff; }
  .btn-green  { background: var(--ok);      color: #000; }
  .btn-yellow { background: var(--warn);    color: #000; }
  .btn-ghost  { border-color: var(--border); color: var(--text); background: transparent; }

  form.inline-form {
    display: flex;
    flex-direction: column;
    gap: .8rem;
    margin-top: .5rem;
  }

  form.inline-form input[type=text],
  form.inline-form input[type=file],
  form.inline-form textarea {
    width: 100%;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: .55em .8em;
    color: var(--text);
    font-family: var(--mono);
    font-size: .85rem;
    resize: vertical;
  }

  form.inline-form textarea { min-height: 80px; }

  form.inline-form input:focus,
  form.inline-form textarea:focus {
    outline: none;
    border-color: var(--accent);
    box-shadow: 0 0 0 3px rgba(0,229,255,.12);
  }

  .status-row {
    display: flex;
    gap: 1rem;
    flex-wrap: wrap;
  }

  .stat-card {
    flex: 1;
    min-width: 160px;
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 10px;
    padding: 1rem 1.2rem;
  }

  .stat-card .label { font-size: .75rem; color: var(--muted); text-transform: uppercase; letter-spacing: .07em; }
  .stat-card .value { font-family: var(--mono); font-size: 1.35rem; font-weight: 700; color: var(--accent); margin-top: .2rem; }

  pre {
    background: var(--bg);
    border: 1px solid var(--border);
    border-radius: 8px;
    padding: .9rem 1rem;
    font-family: var(--mono);
    font-size: .8rem;
    overflow-x: auto;
    color: #a6e3a1;
    margin-top: .6rem;
  }

  .notice {
    background: rgba(0,229,255,.08);
    border: 1px solid rgba(0,229,255,.25);
    border-radius: 8px;
    padding: .7rem 1rem;
    font-family: var(--mono);
    font-size: .82rem;
    color: var(--accent);
    margin-top: .6rem;
  }
</style>
</head>
<body>
<header>
  <h1>⚡ webserv CGI tester</h1>
  <p>42 project · PHP <?= PHP_VERSION ?> · <?= h($method) ?> <?= env('REQUEST_URI') ?></p>
</header>
<main>

<?php // ── Status overview ──────────────────────────────────── ?>
<section>
  <h2>🟢 Request Overview</h2>
  <div class="status-row">
    <div class="stat-card"><div class="label">Method</div><div class="value"><?= env('REQUEST_METHOD') ?></div></div>
    <div class="stat-card"><div class="label">Protocol</div><div class="value"><?= env('SERVER_PROTOCOL') ?></div></div>
    <div class="stat-card"><div class="label">Server Port</div><div class="value"><?= env('SERVER_PORT') ?></div></div>
    <div class="stat-card"><div class="label">PHP SAPI</div><div class="value"><?= h(php_sapi_name()) ?></div></div>
    <div class="stat-card"><div class="label">Content-Length</div><div class="value"><?= env('CONTENT_LENGTH') ?: '0' ?></div></div>
  </div>
  <?php if ($action): ?>
    <div class="notice">▶ action = <strong><?= h($action) ?></strong></div>
  <?php endif; ?>
</section>

<?php // ── CGI Environment vars ─────────────────────────────── ?>
<section>
  <h2>🌍 CGI Environment Variables</h2>
  <?= table([
    'REQUEST_METHOD'    => $_SERVER['REQUEST_METHOD']    ?? '',
    'REQUEST_URI'       => $_SERVER['REQUEST_URI']       ?? '',
    'QUERY_STRING'      => $_SERVER['QUERY_STRING']      ?? '',
    'PATH_INFO'         => $_SERVER['PATH_INFO']         ?? '',
    'SCRIPT_FILENAME'   => $_SERVER['SCRIPT_FILENAME']   ?? '',
    'SCRIPT_NAME'       => $_SERVER['SCRIPT_NAME']       ?? '',
    'SERVER_NAME'       => $_SERVER['SERVER_NAME']       ?? '',
    'SERVER_PORT'       => $_SERVER['SERVER_PORT']       ?? '',
    'SERVER_PROTOCOL'   => $_SERVER['SERVER_PROTOCOL']   ?? '',
    'SERVER_SOFTWARE'   => $_SERVER['SERVER_SOFTWARE']   ?? '',
    'REMOTE_ADDR'       => $_SERVER['REMOTE_ADDR']       ?? '',
    'REMOTE_PORT'       => $_SERVER['REMOTE_PORT']       ?? '',
    'GATEWAY_INTERFACE' => $_SERVER['GATEWAY_INTERFACE'] ?? '',
    'CONTENT_TYPE'      => $_SERVER['CONTENT_TYPE']      ?? '',
    'CONTENT_LENGTH'    => $_SERVER['CONTENT_LENGTH']    ?? '',
    'HTTP_HOST'         => $_SERVER['HTTP_HOST']         ?? '',
    'HTTP_USER_AGENT'   => $_SERVER['HTTP_USER_AGENT']   ?? '',
    'HTTP_ACCEPT'       => $_SERVER['HTTP_ACCEPT']       ?? '',
    'HTTP_CONNECTION'   => $_SERVER['HTTP_CONNECTION']   ?? '',
    'HTTP_COOKIE'       => $_SERVER['HTTP_COOKIE']       ?? '',
  ]) ?>
</section>

<?php // ── GET params ───────────────────────────────────────── ?>
<section>
  <h2>🔎 GET Parameters</h2>
  <?= table($_GET) ?>
  <form class="inline-form" method="GET" style="margin-top:1rem">
    <input type="hidden" name="action" value="custom_get">
    <input type="text" name="foo" placeholder="foo (param name)">
    <input type="text" name="bar" placeholder="bar (param name)">
    <button class="btn btn-cyan" type="submit">Send GET request</button>
  </form>
</section>

<?php // ── POST params ──────────────────────────────────────── ?>
<section>
  <h2>📨 POST Parameters (application/x-www-form-urlencoded)</h2>
  <?= table($_POST) ?>
  <form class="inline-form" method="POST">
    <input type="text"   name="username" placeholder="username">
    <input type="text"   name="message"  placeholder="message">
    <button class="btn btn-pink" type="submit">Send POST request</button>
  </form>
</section>

<?php // ── All HTTP request headers ─────────────────────────── ?>
<section>
  <h2>📋 HTTP Request Headers</h2>
  <?php
    $headers = [];
    foreach ($_SERVER as $k => $v) {
        if (str_starts_with($k, 'HTTP_')) {
            $name = str_replace('_', '-', substr($k, 5));
            $headers[$name] = $v;
        }
    }
    echo table($headers);
  ?>
</section>

<?php // ── PHP info (minimal) ───────────────────────────────── ?>
<section>
  <h2>ℹ️ PHP Info (summary)</h2>
  <?= table([
    'PHP Version'    => PHP_VERSION,
    'SAPI'           => php_sapi_name(),
    'OS'             => PHP_OS_FAMILY,
    'int size'       => PHP_INT_SIZE . ' bytes',
    'max_exec_time'  => ini_get('max_execution_time') . 's',
    'upload_max_filesize' => ini_get('upload_max_filesize'),
    'post_max_size'  => ini_get('post_max_size'),
    'memory_limit'   => ini_get('memory_limit'),
    'tmp_dir'        => sys_get_temp_dir(),
    'date.timezone'  => ini_get('date.timezone') ?: 'not set',
  ]) ?>
</section>

</main>
</body>
</html>

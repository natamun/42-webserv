#!/usr/bin/php-cgi
<?php
$method = $_SERVER['REQUEST_METHOD'] ?? '';
$query  = $_SERVER['QUERY_STRING'] ?? '';
$body   = '';

if ($method === 'POST') {
    $len  = intval($_SERVER['CONTENT_LENGTH'] ?? 0);
    $body = $len > 0 ? fread(STDIN, $len) : '';
}

header('Content-Type: text/html');
?>
<html><body>
<h1>PHP CGI Test</h1>
<p><b>METHOD:</b> <?= htmlspecialchars($method) ?></p>
<p><b>QUERY_STRING:</b> <?= htmlspecialchars($query) ?></p>
<?php if ($body): ?>
<p><b>BODY:</b> <?= htmlspecialchars($body) ?></p>
<?php endif; ?>
</body></html>

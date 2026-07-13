#!/usr/bin/php-cgi
<?php
$method = $_SERVER['REQUEST_METHOD'] ?? '';

$fields = $_POST;
if (empty($fields)) {
    $len = intval($_SERVER['CONTENT_LENGTH'] ?? 0);
    $body = $len > 0 ? fread(STDIN, $len) : '';
    parse_str($body, $fields);
}

header('Content-Type: text/html');
?>
<html><body>
<h1>POST form received (php)</h1>
<p>METHOD: <?= htmlspecialchars($method) ?></p>
<?php if (!empty($fields)): ?>
<ul>
<?php foreach ($fields as $key => $value): ?>
    <li><b><?= htmlspecialchars($key) ?></b> = <?= htmlspecialchars($value) ?></li>
<?php endforeach; ?>
</ul>
<?php else: ?>
<p>(no form fields received)</p>
<?php endif; ?>
</body></html>

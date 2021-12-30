--TEST--
priority_sampling default decision retained
--ENV--
DD_TRACE_SAMPLING_RULES=[{"sample_rate": 1}]
DD_TRACE_GENERATE_ROOT_SPAN=1
--FILE--
<?php
\DDTrace\set_priority_sampling(DD_TRACE_PRIORITY_SAMPLING_UNSET, true);

\DDTrace\get_priority_sampling(true);

$root = \DDTrace\root_span();

if (!isset($root->metrics["_dd.rule_psr"])) {
    echo "OK";
} else {
    echo "metrics[_dd.rule_psr] = {$root->metrics["_dd.rule_psr"]}\n";
}
?>
--EXPECT--
OK

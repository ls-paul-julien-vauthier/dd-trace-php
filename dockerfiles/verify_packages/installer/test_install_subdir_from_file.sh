#!/usr/bin/env sh

set -e

. "$(dirname ${0})/utils.sh"

# Initially no ddtrace
assert_no_ddtrace

if [ "$CIRCLECI" = "true" ]; then
    "SKIPPED: this test runs only in CI as it requires the .tar.gz at a specific path"
    exit 0
fi

new_version="0.68.0"
curl -L -o /tmp/downloaded.tar.gz "https://github.com/DataDog/dd-trace-php/releases/download/${new_version}/dd-library-php-x86_64-linux-gnu.tar.gz"

# Install using the php installer
php dd-library-php-setup.php --php-bin php --file /tmp/downloaded.tar.gz

# Just check installation, not the version as it is not deterministic.
if [ -z "$(php -m | grep ddtrace)" ]; then
    echo "\nError: ddtrace is not installed\n---\n$(php -v)\n---\n"
    exit 1
else
    echo "\nOk: ddtrace is installed\n"
fi

assert_file_exists /opt/datadog/dd-library/*/dd-trace-sources/bridge/dd_wrap_autoloader.php

assert_request_init_hook_exists

#!/usr/bin/env sh

set -e

OS_VERSION=$(source /etc/os-release; echo $VERSION_ID)

# Enable epel repo
rpm -Uvh https://dl.fedoraproject.org/pub/epel/epel-release-latest-${OS_VERSION}.noarch.rpm

# Installing pre-requisites
yum install -y wget nginx httpd

# Installing php
rpm -Uvh http://rpms.famillecollet.com/enterprise/remi-release-${OS_VERSION}.rpm
yum --enablerepo=remi-php${PHP_MAJOR}${PHP_MINOR} install -y \
    php-cli \
    php-fpm \
    php-opcache \
    php-pear \
    mod_php

# Preparing PHP-FPM
# Where php-fpm PID will be stored
mkdir -p /run/php-fpm
# For cases when it defaults to UDS
sed -i 's/^listen = .*$/listen = 9000/g' /etc/php-fpm.d/www.conf
# Passing envs to php-fpm process directly for simplicity. Note that on PHP 5.4 clear_env does not appear in www not
# even commented, so we remove potential existing line and add it at the end of the config file.
sed -i 's/^;*clear_env.*$//g' /etc/php-fpm.d/www.conf
echo 'clear_env = no' >> /etc/php-fpm.d/www.conf
# Enabling logs
sed -i 's/^;*catch_workers_output.*$/catch_workers_output = yes/g' /etc/php-fpm.d/www.conf

# Preparing NGINX
groupadd www-data
adduser -M --system -g www-data www-data
# Note: ignoring alias cp='cp -i' via leading `\` to avoid interactive mode for override operation.
\cp $(pwd)/dockerfiles/verify_packages/nginx.conf /etc/nginx/nginx.conf

# Installing dd-trace-php
INSTALL_TYPE="${INSTALL_TYPE:-php_installer}"
if [ "$INSTALL_TYPE" = "native_package" ]; then
    echo "Installing dd-trace-php using the OS-specific package installer"
    rpm -ivh $(pwd)/build/packages/*.rpm
else
    echo "Installing dd-trace-php using the new PHP installer"
    php dd-library-php-setup.php --file $(pwd)/build/packages/dd-library-php-x86_64-linux-gnu.tar.gz --php-bin all
fi

# Starting services
php-fpm
sleep 0.5
nginx
sleep 0.5
httpd
sleep 0.5

echo "Installation completed successfully"

<?php
/**
 * @file
 * Create a database and the user based on the preferences in the database file.
 */

/**
 * Implements hook_drush_command().
 */
function sqlinit_drush_command() {
  $items['sql-init'] = array(
    'bootstrap' => DRUSH_BOOTSTRAP_DRUPAL_CONFIGURATION,
    'description' => 'Create a database and the user based on the preferences in the database file',
    'callback' => 'drush_sqlinit_sql_inti',
  );

  return $items;
}

/**
 * Create a database and a user.
 *
 * This currently only works with mysql.
 */
function drush_sqlinit_sql_inti() {

  if (drush_get_option('db-url', FALSE)) {
    $db_spec['db-url'] = $GLOBALS['db_url'];
  }

  if (!isset($db_spec)) {
    $db_spec = drush_sql_conf();
  }

  drush_backend_set_result($db_spec);

  if ($db_spec['driver'] != 'mysql') {
    drush_log(dt('Unable to connect to the driver, mysql is the only supported driver'), 'error');
    return FALSE;
  }

  $root_url = 'mysql://root@' . $db_spec['host'];
  // Connect as root.
  $root = drush_convert_db_from_db_url($root_url);

  mysql_connect($root['host'], $root['username'], $root['password']);

  // Create the database.
  mysql_query('CREATE DATABASE `' .  $db_spec['database'] . '`');

  // Make sure our user can write to it.
  $grant_query = 'GRANT ALL PRIVILEGES ON `' . $db_spec['database'] . '`.* TO \'' . $db_spec['username'] . '\'@\'localhost\'';
  if (isset($db_spec['password'])) {
    $grant_query .= ' IDENTIFIED BY \'' . $db_spec['password'] . '\'';
  }
  mysql_query($grant_query);

  return drush_log(dt('Database (@database) and user (@user) created', array('@user' => $db_spec['username'], '@database' => $db_spec['database'])), 'success');
}

<?php

/**
 * @file
 * Theme functions for domains module.
 */

/**
 * Theme domains_result_list
 * Display form result from domain registration form
 */
function theme_domains_result_list($variables) {
  $results = $variables['results'];
  
  $list = array();
  foreach ($results as $domain => $available) {
    if ($available) {
      $status = t('is');
      $class = array('notice', 'ok');
    }
    else {
      $status = t('is NOT');
      $class = array('error');
    }
    $tokens = array('@domain' => $domain, '@available' => $status);
    $list[] = array('data' => t('@domain @available available.', $tokens), 'class' => $class);
  }
  
  $title = t('Search Results');
  return theme('item_list', array('items' => $list, 'title' => $title));
}

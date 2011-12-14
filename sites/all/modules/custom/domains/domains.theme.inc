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

/**
 * Theme domain dns record form
 */
function theme_domains_dns_records_form($variables) {
  $element = $variables['element'];
  $header = array(t('Record'), t('Type'), t('Value'), t('Priority'));
  $rows = array();

  foreach (element_children($element) as $index) {
    $row = array();
    $row[] = render($element[$index]['record']);
    $row[] = render($element[$index]['type']);
    $row[] = render($element[$index]['value']);
    $row[] = render($element[$index]['priority']);

    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'sticky' => FALSE,
  ));
}

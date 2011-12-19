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
  $header = array(t('Name'), t('Type'), t('Value'), t('Priority'), t('Delete'));
  $rows = array();

  foreach (element_children($element) as $index) {
    $data = array();
    $data[] = render($element[$index]['name']);
    $data[] = render($element[$index]['type']);
    $data[] = render($element[$index]['value']);
    $data[] = render($element[$index]['priority']);
    $data[] = render($element[$index]['delete']);

    $row = array(
      'data' => $data,
    );

    $rows[] = $row;
  }

  return theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'sticky' => FALSE,
  ));
}

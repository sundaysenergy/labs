<?php
/**
 * @file
 * domain_features.views_default.inc
 */

/**
 * Implements hook_views_default_views().
 */
function domain_features_views_default_views() {
  $export = array();

  $view = new view;
  $view->name = 'domain_manage';
  $view->description = '';
  $view->tag = 'default';
  $view->base_table = 'domain';
  $view->human_name = 'Domain Manage';
  $view->core = 7;
  $view->api_version = '3.0';
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

  /* Display: Master */
  $handler = $view->new_display('default', 'Master', 'default');
  $handler->display->display_options['title'] = 'Your Domains Expiring Soon';
  $handler->display->display_options['access']['type'] = 'none';
  $handler->display->display_options['cache']['type'] = 'none';
  $handler->display->display_options['query']['type'] = 'views_query';
  $handler->display->display_options['query']['options']['query_comment'] = FALSE;
  $handler->display->display_options['exposed_form']['type'] = 'basic';
  $handler->display->display_options['pager']['type'] = 'full';
  $handler->display->display_options['pager']['options']['items_per_page'] = '100';
  $handler->display->display_options['pager']['options']['offset'] = '0';
  $handler->display->display_options['pager']['options']['id'] = '0';
  $handler->display->display_options['pager']['options']['expose']['items_per_page_options_all'] = 0;
  $handler->display->display_options['style_plugin'] = 'table';
  $handler->display->display_options['style_options']['columns'] = array(
    'id' => 'id',
    'name' => 'name',
    'expiration' => 'expiration',
  );
  $handler->display->display_options['style_options']['default'] = '-1';
  $handler->display->display_options['style_options']['info'] = array(
    'id' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'name' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
    'expiration' => array(
      'sortable' => 0,
      'default_sort_order' => 'asc',
      'align' => '',
      'separator' => '',
      'empty_column' => 0,
    ),
  );
  $handler->display->display_options['style_options']['override'] = 1;
  $handler->display->display_options['style_options']['sticky'] = 0;
  $handler->display->display_options['style_options']['empty_table'] = 0;
  /* Field: Domain: Domain ID */
  $handler->display->display_options['fields']['id']['id'] = 'id';
  $handler->display->display_options['fields']['id']['table'] = 'domain';
  $handler->display->display_options['fields']['id']['field'] = 'id';
  $handler->display->display_options['fields']['id']['label'] = '';
  $handler->display->display_options['fields']['id']['exclude'] = TRUE;
  $handler->display->display_options['fields']['id']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['id']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['id']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['id']['alter']['external'] = 0;
  $handler->display->display_options['fields']['id']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['id']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['id']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['id']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['id']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['id']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['id']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['id']['alter']['html'] = 0;
  $handler->display->display_options['fields']['id']['element_label_colon'] = FALSE;
  $handler->display->display_options['fields']['id']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['id']['hide_empty'] = 0;
  $handler->display->display_options['fields']['id']['empty_zero'] = 0;
  $handler->display->display_options['fields']['id']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['id']['separator'] = '';
  $handler->display->display_options['fields']['id']['format_plural'] = 0;
  /* Field: Domain: Domain Name */
  $handler->display->display_options['fields']['name']['id'] = 'name';
  $handler->display->display_options['fields']['name']['table'] = 'domain';
  $handler->display->display_options['fields']['name']['field'] = 'name';
  $handler->display->display_options['fields']['name']['label'] = 'Domain name';
  $handler->display->display_options['fields']['name']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['name']['alter']['make_link'] = 1;
  $handler->display->display_options['fields']['name']['alter']['path'] = 'domain/[id]';
  $handler->display->display_options['fields']['name']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['name']['alter']['external'] = 0;
  $handler->display->display_options['fields']['name']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['name']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['name']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['name']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['name']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['name']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['name']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['name']['alter']['html'] = 0;
  $handler->display->display_options['fields']['name']['element_label_colon'] = 0;
  $handler->display->display_options['fields']['name']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['name']['hide_empty'] = 0;
  $handler->display->display_options['fields']['name']['empty_zero'] = 0;
  $handler->display->display_options['fields']['name']['hide_alter_empty'] = 1;
  /* Field: Domain: Expiration date */
  $handler->display->display_options['fields']['expiration']['id'] = 'expiration';
  $handler->display->display_options['fields']['expiration']['table'] = 'domain';
  $handler->display->display_options['fields']['expiration']['field'] = 'expiration';
  $handler->display->display_options['fields']['expiration']['alter']['alter_text'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['make_link'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['absolute'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['external'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['replace_spaces'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['trim_whitespace'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['nl2br'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['word_boundary'] = 1;
  $handler->display->display_options['fields']['expiration']['alter']['ellipsis'] = 1;
  $handler->display->display_options['fields']['expiration']['alter']['strip_tags'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['trim'] = 0;
  $handler->display->display_options['fields']['expiration']['alter']['html'] = 0;
  $handler->display->display_options['fields']['expiration']['element_label_colon'] = 1;
  $handler->display->display_options['fields']['expiration']['element_default_classes'] = 1;
  $handler->display->display_options['fields']['expiration']['hide_empty'] = 0;
  $handler->display->display_options['fields']['expiration']['empty_zero'] = 0;
  $handler->display->display_options['fields']['expiration']['hide_alter_empty'] = 1;
  $handler->display->display_options['fields']['expiration']['date_format'] = 'long';
  /* Contextual filter: Domain: Creator ID */
  $handler->display->display_options['arguments']['uid']['id'] = 'uid';
  $handler->display->display_options['arguments']['uid']['table'] = 'domain';
  $handler->display->display_options['arguments']['uid']['field'] = 'uid';
  $handler->display->display_options['arguments']['uid']['default_action'] = 'default';
  $handler->display->display_options['arguments']['uid']['default_argument_type'] = 'current_user';
  $handler->display->display_options['arguments']['uid']['default_argument_skip_url'] = 0;
  $handler->display->display_options['arguments']['uid']['summary']['number_of_records'] = '0';
  $handler->display->display_options['arguments']['uid']['summary']['format'] = 'default_summary';
  $handler->display->display_options['arguments']['uid']['summary_options']['items_per_page'] = '25';
  $handler->display->display_options['arguments']['uid']['break_phrase'] = 0;
  $handler->display->display_options['arguments']['uid']['not'] = 0;
  /* Filter criterion: Domain: Expiration date */
  $handler->display->display_options['filters']['expiration']['id'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['table'] = 'domain';
  $handler->display->display_options['filters']['expiration']['field'] = 'expiration';

  /* Display: Domains */
  $handler = $view->new_display('page', 'Domains', 'page');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Manage Your Domains';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  $handler->display->display_options['path'] = 'domain/list';
  $handler->display->display_options['menu']['type'] = 'default tab';
  $handler->display->display_options['menu']['title'] = 'Manage Your Domain';
  $handler->display->display_options['menu']['weight'] = '-10';
  $handler->display->display_options['tab_options']['type'] = 'normal';
  $handler->display->display_options['tab_options']['title'] = 'Manage your domains';
  $handler->display->display_options['tab_options']['weight'] = '-1';

  /* Display: Domain Expire Soon */
  $handler = $view->new_display('page', 'Domain Expire Soon', 'page_1');
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Domain: Expiration date */
  $handler->display->display_options['filters']['expiration']['id'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['table'] = 'domain';
  $handler->display->display_options['filters']['expiration']['field'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['operator'] = 'between';
  $handler->display->display_options['filters']['expiration']['value']['min'] = '-30 day';
  $handler->display->display_options['filters']['expiration']['value']['max'] = '+0 day';
  $handler->display->display_options['filters']['expiration']['value']['value'] = '-30 day';
  $handler->display->display_options['filters']['expiration']['value']['type'] = 'offset';
  $handler->display->display_options['filters']['expiration']['expose']['operator_id'] = 'expiration_op';
  $handler->display->display_options['filters']['expiration']['expose']['label'] = 'Expiration date';
  $handler->display->display_options['filters']['expiration']['expose']['use_operator'] = FALSE;
  $handler->display->display_options['filters']['expiration']['expose']['operator'] = 'expiration_op';
  $handler->display->display_options['filters']['expiration']['expose']['identifier'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['expose']['required'] = FALSE;
  $handler->display->display_options['filters']['expiration']['expose']['remember'] = FALSE;
  $handler->display->display_options['filters']['expiration']['expose']['multiple'] = FALSE;
  $handler->display->display_options['path'] = 'domain/expiring-soon';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Domain Expiring Soon';
  $handler->display->display_options['menu']['weight'] = '1';

  /* Display: Domain Expired */
  $handler = $view->new_display('page', 'Domain Expired', 'page_2');
  $handler->display->display_options['defaults']['title'] = FALSE;
  $handler->display->display_options['title'] = 'Your Domains Expired';
  $handler->display->display_options['defaults']['filters'] = FALSE;
  /* Filter criterion: Domain: Expiration date */
  $handler->display->display_options['filters']['expiration']['id'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['table'] = 'domain';
  $handler->display->display_options['filters']['expiration']['field'] = 'expiration';
  $handler->display->display_options['filters']['expiration']['operator'] = '<';
  $handler->display->display_options['filters']['expiration']['value']['value'] = '+0 day';
  $handler->display->display_options['filters']['expiration']['value']['type'] = 'offset';
  $handler->display->display_options['path'] = 'domain/expired';
  $handler->display->display_options['menu']['type'] = 'tab';
  $handler->display->display_options['menu']['title'] = 'Domain Expired';
  $handler->display->display_options['menu']['weight'] = '3';
  $export['domain_manage'] = $view;

  return $export;
}

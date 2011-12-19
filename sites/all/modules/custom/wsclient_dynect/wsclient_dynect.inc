<?php

/**
 * Get DNS record by URI with cached enabled
 */
function _wsclient_dynect_get_record($service, $record_uri, $reset = FALSE) {
  $cid = "dynect:$record_uri";
  $cache = cache_get($cid);
  $record = FALSE;

  if (!$cache || $reset) {
    $record = _wsclient_dynect_record_query($record_uri);
    $record_detail = $service->get_record($record['record_type'], $record['query']);

    if ($record_detail['status'] == 'success') {
      $record += $record_detail['data'];

      cache_set($cid, $record, 'cache', time() + 3600);
    }
    else {
      $record = FALSE;
    }
  }
  else {
    $record = $cache->data;
  }

  return $record;
}

/**
 * Return record query string from record URL 
 */
function _wsclient_dynect_record_query($uri) {
  $record = str_replace('/REST/', '', $uri);
  $tmp = explode('/', $record);
  $record_type = $tmp[0];
  array_shift($tmp);

  $types = _wsclient_dynect_support_type();
  if (isset($types[$record_type])) {
    $type = $types[$record_type];
  }
  else {
    // Remove the "Record" suffix.
    $type = substr($record_type, 0, -6);
  }

  return array('record_type' => $record_type, 'query' => implode('/', $tmp), 'type' => $type, "uri" => $uri);
}

/**
 * Resolve DynECT Nameserver IP Address.
 */
function _wsclient_dynect_resolve_ip($domain) {
  if (preg_match('/^ns(\d)\.p(\d+)\.dynect\.net$/', $domain, $matches)) {
    $pool = array(
      1 => '208.78.70.',
      2 => '204.13.250.',
      3 => '208.78.71.',
      4 => '204.13.251.',
    );
    if (isset($pool[$matches[1]])) {
      return $pool[$matches[1]] . intval($matches[2]);
    }
  }
}

function _wsclient_dynect_support_type() {
  return array(
    'ARecord' => 'A',
    'AAAARecord' => 'AAAA',
    'CNAMERecord' => 'CNAME',
    'MXRecord' => 'MX',
    'SOARecord' => 'SOA',
    'SPFRecord' => 'SPF',
    'SRVRecord' => 'SRV',
    'TXTRecord' => 'TXT',
  );
}

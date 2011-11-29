<?php
ignore_user_abort(1);

define('GIT_BRANCH', 'master');
define('UNF_SUBDOMAIN', 'sundaysenergy');
define('UNF_PID', 37323); // Project ID
define('UNF_RID', 26); // Repository ID
define('CAMP_SUBDOMAIN', 'sundaysenergy');
define('CAMP_TOKEN', 'a3241be299ce1b45b2d5c1d2cb0752abed896ee0');
define('CAMP_PW', 'cc2de8a01a7c5d662');
//To get room_id, login to your CF room and look in the address bar (after /room)
define('CAMP_RM', 435558); // Room ID

$master_server = 'www.acfdev.com';
$other_servers = array();

$file_name = '/git-post-commit.php';
$request = $HTTP_RAW_POST_DATA;
$paste = array();

$revision = $author = $message = $repo = 'n/a';
if ($request) {
  if ($master_server == $_SERVER['SERVER_NAME'] && !empty($other_servers)) {
    foreach ($other_servers as $server_name) {
      $url = $server_name . $file_name;
      $ch = curl_init();
      curl_setopt($ch, CURLOPT_URL, $url);
      curl_setopt($ch, CURLOPT_HTTPHEADER, array("Content-Type: text/xml"));
      curl_setopt($ch, CURLOPT_POST, 1);
      curl_setopt($ch, CURLOPT_POSTFIELDS, "$request");
      curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
      $paste[$server_name] = curl_exec($ch);
      curl_close($ch);
    }
  }
  $reader = new XMLReader();
  $reader->XML($request);

  while ($reader->read()) {
    if ($reader->name == "revision" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $revision = $reader->value;
    }
    
    if ($reader->name == "author-name" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $author = $reader->value;
    }
    
    if ($reader->name == "message" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $message = $reader->value;
    }
    if ($reader->name == "id" && $reader->nodeType == XMLReader::ELEMENT) {
      $reader->read();
      $repo = $reader->value;
    }
  }
  $reader->close();
}

$git = 'pull ' . GIT_BRANCH;
$paste[$git] = exec('/usr/bin/git pull');
if (!$paste['pull master'] || !is_string($paste[$git])) {
  $paste['pull master'] = 'NO GIT Response';
}
//$paste['globals'] = print_r($GLOBALS, true);
if ($author && is_string($author)) {
  $paste['author'] = $author;
}
if ($revision && is_string($revision)) {
  $paste['revision'] = $revision;
}
if ($message && is_string($message)) {
  $paste['message'] = $message;
}

include('icecube.class.php');
$url = 'https://' . UNF_SUBDOMAIN . 'campfirenow.com';	// Use full URL, http://[account].campfirenow.com
$repo_l = ' https://sundaysenergy.unfuddle.com/a#/projects/' . UNF_PID . '/repositories/' . UNF_RID . '/commit?commit=' . $revision;

$icecube = new icecube($url, CAMP_TOKEN);
$icecube->login();
//$icecube->joinRoom($room_id);
//$icecube->leaveRoom($room_id);
$say = $author . " commit: '" . $message . "'";
$say2 = ' Site status: ' . $paste['pull master'];
$icecube->speak($say . $repo_l . $say2, CAMP_RM);
// $icecube->speak($say2, $room_acf);
// $icecube->speak(print_r($paste, TRUE), $room_dev, TRUE);
// $icecube->speak($say . $repo_l . $say2, $room_se);
// $icecube->speak($say2, $room_se);

$icecube->logout();

$fh = fopen("gitpull.txt", 'a') or die("can't open file");
fwrite($fh, print_r($paste, TRUE));
fclose($fh);

echo $paste['pull master'];
?>

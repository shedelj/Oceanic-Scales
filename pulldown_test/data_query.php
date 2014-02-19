<?php
//include 'fake.php';


//header('content-type: text/plain');

//refreshes the page every 30 minutes
// $url=$_SERVER['REQUEST_URI'];
// header("Refresh: 1800; URL=$url"); 

$contents2 = file_get_contents('http://www.mbari.org/lobo/cgi-bin/GetLOBOData.exe?timeunit=SPECIFY&min_date=10252012&max_date=11252013&dataquality=GQ&station=M1SURF.TXT&x=day&y=nitrate&y=temperature&y=pH_Total_InSituT&y=pH_Total_20C&xmin=&xmax=&ymin=&ymax=&ystack=OFF&data_format=Text&submit=SEND');

//any multiple white spaces into 1
$contents2 = preg_replace('/\s(1,)/', ' ', $contents2);
$data_file = fopen("data.txt", 'w+');

//data
$data2 = preg_split('/<br>/', $contents2);
$tag = preg_split('/,/', $data2[1]);

for ($i=0; $i < count($tag)-1; $i++){ 
	if($i != 1) print_r( trim($tag[$i]) . ", " );
}
print_r( trim($tag[count($tag)-1]) . "\n\n" );

$dataCount = count($data2)-3;

for ($ix = 2; $ix < $dataCount; $ix++) {
	$tmp = $data2[$ix];
	$tmp = str_replace(' ', '', $tmp);
	$match_data = preg_split('/,/', $tmp);
	// print_r($match_data);
	if (count($match_data) == 4) {
		$index = 2;
	}else{
		$index = 3;
	}
	// $tmp = preg_split('/ /', $match_data[0]);
	// $time = $tmp[1];
	// $tmp = preg_split('/\//', $tmp[0]);
	// $month = trim($tmp[0]);
	// $day = $tmp[1];
	// $year = $tmp[2];
	// $nitrate = preg_split('/, /', $match_data[1])[1];
	$temp = $match_data[$index];
	// $pHi = $match_data[4];
	// $pHc = $match_data[4];

	// $pH = $match_data[0];
	// $nitrate = $match_data[1] . "\r\n";
	// $salinity = $match_data[2] . "\r\n";
	// $temperature = $match_data[3] . "\r\n";
	// fwrite($pH_file, $pH);
	
	// $tmp = array('month' => $month, 'day' => $day, 'year' => $year,
	// 				'time' => $time, 'nitrate' => $nitrate, 'temp' => $temp,
	// 				'pHi' => $pHi, 'pHc' => $pHc);
	// print_r(json_encode($tmp)."\n");
	// print_r($month . "-" . $day . "-" . $year . " " . $time . "\t" .
	// 			$nitrate . "\t" . $temp . "\t" . $pHi . "\t" . $pHc."\n");

	if ($temp > 100)continue;

	if ($tempArray == null){
		$tempArray = array($temp);
	} else {
		array_push($tempArray, $temp);
	}
	
	$curr = $temp;
}

$aCount = count($tempArray);
$sum = array_sum($tempArray);
$avg = ($sum/$aCount);
sort($tempArray, 1);
$min = $tempArray[0];
$max = $tempArray[$aCount-1];
print_r("aCount: " . $aCount . "\t" . "sum: " . $sum . "\t" .
			"avg: " . $avg . "\t" . "min: " . $min . "\t" . "max: " . $max);
include 'php_serial.class.php';

//fwrite($data_file, $min);
//fwrite($data_file, $avg);
//fwrite($data_file, $max);

//$serial = new phpSerial;

//$serial->deviceSet("/dev/ttyACM0");
//$serial->deviceSet("/dev/tty.usbmodem3a231");
//$serial->confBaudRate(9600);
//$serial->confParity("none");
//$serial->confCharacterLength(8);
//$serial->confStopBits(1);
//$serial->confFlowControl("none");

//$serial->deviceOpen();
$range = $max - $min;
$curr = ($curr - $min) / ($max - $min) * (255 - 0) + 0;

//settype($curr, "int");
//print_r("$curr");
fwrite($data_file, $curr);
//$serial->sendMessage($curr);

//$serial->deviceClose();

?>

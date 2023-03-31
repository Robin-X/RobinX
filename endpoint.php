
<?php

print "<B>Uploaded file processing</B><BR>\n";

function writepayload() { // write payload to file
        $tempfile = tempnam(sys_get_temp_dir(),"latex");
        $message  = $_POST["payload"];
        $temph    = fopen($tempfile, "w");

        if($temph===false)
                throw new exception("file open for $tempfile failed");
        if(fwrite($temph,$message)==false)
                throw new exception("file write for $tempfile failed");
        fclose($temph);

        print "written $tempfile<BR>\n";
        return $tempfile;
}
function readpayload($tempfile) { // reopen and read file
        $size = filesize($tempfile);
        $h    = fopen($tempfile,"r");
        print "reading $size bytes from $tempfile<BR>\n";

        if($h===false)
                throw new exception("file open for $tempfile failed");
        $buf  = fread($h,$size);
        if($buf===false)
                throw new exception("file read for $tempfile failed");
        fclose($h);
        return $buf;
}

if(array_key_exists("payload",$_FILES)) {
        // file uploaded
        $file = $_FILES["payload"]["tmp_name"];
} else {
        // data posted, write to file ourselves
        $file = writepayload();
}

// de geuploade data steekt nu in het bestand $file, je kan via een system() hier een commando op loslaten of het vanuit het PHP script uitlezen voor verdere verwerking
$buf=readpayload($file);
print "content: $buf<BR>\n";

// unlink($file); // delete temp file

?>

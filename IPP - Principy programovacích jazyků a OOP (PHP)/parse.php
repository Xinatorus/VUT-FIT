<?php
/*
	PARAMS:
	--help		- samostatne

	OUTPUT:	xml
		<?xml version="1.0" encoding="UTF-8"?>
			<program language="IPPcode18">
				<instruction order="1..n" opcode="OPCODE">
					<arg1 type="TYPE">value</arg1>
				</instruction>
			</program>

	OPCODE:	hodnota operačního kódu je vždy velkými písmeny
	TYPE: 	int, bool, string, label, type, var
	el ARG: 		arg1, arg2, arg3
*/
	$ERR_PARAM = 10;
	$ERR_INPUT = 11;
	$ERR_OUTPUT = 12;
	$ERR_PARSER = 21;
	$ERR = 99;

	$params;
	$n_value = false;
	$help = false;


	$longopts  = array("stats:", "loc", "comments", "help");
	$options = getopt("", $longopts);

	$statFlag = false;
	$helpFlag = false;
	$locFlag = false;
	$comFlag = false;
	$first = "";

	foreach ($options as $key => $value) {
		if($key == "stats") $statFlag = true;
		if($key == "loc"){
			 $locFlag = true;
			 if($first == "") $first = "loc";
		 }
		if($key == "comments"){
			 $comFlag = true;
			 if($first == "") $first = "comments";
		 }
		if($key == "help") $helpFlag = true;
	}

	if($helpFlag && ($statFlag || $locFlag || $comFlag)) exit(10);
	if($helpFlag){
		write_help();
		exit(0);
	}
	if (!$statFlag && ($locFlag || $comFlag)) exit(10);


// REGEXY

	$r_comm = '~^#.*$~'; // na radku je pouze kokmentar
	$r_commafter = '~#.*$~'; // komentar za kodem
	$r_emptyline = '~^\s*$~';
	$r_type = '(bool|int|string)';
	$r_bool = 'bool@(true|false)';
	$r_int = 'int@[-+]?[0-9]+';
	$r_string = 'string@([^\s#\\\\]|(\\\\\d{3}))*'; //string@([^\s#\\\\]|(\\\\\d{3}))*
	$r_symb = '((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)'; // promenna nebo konstanta
	$r_var = '(LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*'; // promenna
	$r_label = '[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*';

// INSTRUCTION SET 34

// (?i)LABEL(?-i)
$r_nop = '^\s*(?i)(CREATEFRAME|PUSHFRAME|POPFRAME|BREAK|RETURN)(?-i)\s*$';

// --label
// 	(?i)(CALL|LABEL|JUMP)(?-i) label
$r_l = '^\s*(?i)(CALL|LABEL|JUMP)(?-i)\s+([a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s*$';

// -- label symb symb
$r_lss = '^\s*(?i)(JUMPIFNEQ|JUMPIFEQ)(?-i)\s+([a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)(\s*((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)){2}\s*$';

// --var
// (?i)(DEFVAR|POPS) var
$r_v = '^\s*(?i)(DEFVAR|POPS)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s*$';

// --var symb
// (?i)(MOVE|INT2CHAR|STRLEN|TYPE|NOT)(?-i) var symb
$r_vs = '^\s*(?i)(MOVE|INT2CHAR|STRLEN|TYPE|NOT)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)\s*$';

// --symb
// (?i)(WRITE|DPRINT|PUSHS)(?-i) symb
$r_s ='^\s*(?i)(WRITE|DPRINT|PUSHS)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)\s*$';

// --var type
// (?i)(READ)(?-i) var type
$r_t = '^\s*(?i)(READ)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s+(bool|int|string)\s*$';

// --var symb symb
// (?i)(ADD|SUB|MUL|IDIV|LT|GT|EQ|AND|OR|STRI2INT|CONCAT|GETCHAR|SETCHAR)(?-i) var symb symb
$r_vss='^\s*(?i)(ADD|SUB|MUL|IDIV|LT|GT|EQ|AND|OR|STRI2INT|CONCAT|GETCHAR|SETCHAR)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)(\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)){2}\s*$';

// Cely jazyk:

	// $r_full = '~(('.$r_nop.')|('.$r_l.')|('.$r_lss.')|('.$r_vs.')|('.$r_s.')|('.$r_t.')|('.$r_vss.'))~';

	$r_full = '/((^\s*(?i)(CREATEFRAME|PUSHFRAME|POPFRAME|BREAK|RETURN)(?-i)\s*$)|(^\s*(?i)(CALL|LABEL|JUMP)(?-i)\s+([a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s*$)|(^\s*(?i)(JUMPIFNEQ|JUMPIFEQ)(?-i)\s+([a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)(\s*((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)){2}\s*$)|(^\s*(?i)(DEFVAR|POPS)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s*$)|(^\s*(?i)(MOVE|INT2CHAR|STRLEN|TYPE|NOT)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)\s*$)|(^\s*(?i)(WRITE|DPRINT|PUSHS)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)\s*$)|(^\s*(?i)(READ)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)\s+(bool|int|string)\s*$)|(\s*(?i)(ADD|SUB|MUL|IDIV|LT|GT|EQ|AND|OR|STRI2INT|CONCAT|GETCHAR|SETCHAR)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]*)(\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)){2}\s*$))/';


	// echo preg_match('/^\s*(?i)(WRITE|DPRINT|PUSHS)(?-i)\s+((LF|TF|GF)@[a-zA-Z_\-\$&%*][a-zA-Z0-9_\-\$&%*]+|bool@(true|false)|int@[-+]?[0-9]+|string@([^\s#\\\\]|(\\\\\d{3}))*)\s*$/','PUSHS string@Nějaké\032UNICODE\032kódy');
	// exit(0);


	$lineCount = 0;
	$order = 1;
	$commentCount = 0;

	while($line = fgets(STDIN)){
		// echo "\n".($lineCount+1)." ";
		if($lineCount == 0){
			if(!preg_match("/^\s*(?i).IPPcode18(?-i)\s*$/",$line)){
				// fwrite(STDERR, "ERR ".$ERR_PARSER.": Parser error\n");
				exit($ERR_PARSER);
			}
			// echo "IPPcode18 OK ";
			fwrite(STDOUT, '<?xml version="1.0" encoding="UTF-8"?>'."\n");
			fwrite(STDOUT,'<program language="IPPcode18">'."\n");
		}else{
			if(preg_match("/^#.*$/",$line)){ // komentar na celem radku
		 		// echo "comme ";
				$lineCount++;
				$commentCount++;
				continue;
		 	}
			if(preg_match("/#.*$/",$line)){ // komentar za instrukci
				// echo "commafter ";
				$line = preg_replace('/#.*$/','',$line);
				$commentCount++;
			}
			if(preg_match("/^\s*$/",$line)){ // prazdny radek
		 		// echo "empty ";
				$lineCount++;
				continue;
		 	}

			if(preg_match($r_full,$line)){
				// echo "OK ";
				fwrite(STDOUT,parseInstruction($line,$order));
				$order++;
		 	}else exit($ERR_PARSER);

		}

		$lineCount++;
	}
	fwrite(STDOUT,'</program>'."\n");

	if($statFlag){
		$file = fopen($options["stats"],"w");
		if($first == "loc"){
			fwrite($file,($order-1)."\n");
			if($comFlag) fwrite($file,($commentCount)."\n");
		}else{
			fwrite($file,($commentCount)."\n");
			if($locFlag) fwrite($file,($order-1)."\n");
		}
		fclose($file);
	}


	exit(0);

/*
	<instruction order="1..n" opcode="OPCODE">
		<arg1 type="TYPE">value</arg1>
	</instruction>


	OPCODE:	hodnota operačního kódu je vždy velkými písmeny
	TYPE: 	int, bool, string, label, type, var
	el ARG: 		arg1, arg2, arg3
*/

//zpracovani jednotlive instrukce
function parseInstruction($line,$order){
	$match;
	// echo $line;
	preg_match("/^\s*(\S+)/",$line,$match);
	$opcode=strtoupper($match[0]);

	$return = '<instruction order="'.$order.'" opcode="'.$opcode.'">'."\n";

	//zpracovani argumentu
	$arg = preg_replace('/((^\s*\S+)|\s*$)/','',$line);

	preg_match_all('/\s+\S+/',$arg,$match);
	// var_dump($match[0]);
	$i = 1;
	foreach ($match[0] as $key => $value) {
		$return .= parseArg(preg_replace('/^\s*/','',$value),$i);
		$i++;
	}

	//ukonceni instrukce
	$return .= '</instruction>'."\n";
	return $return;
}

function parseArg($arg,$i){

	$type = '';
	$value = '';
	$match;
	// OK type (int,string,bool)
	// OK variables
	// var
	// label

	if(preg_match('/^(int|string|bool)$/',$arg)){
		$type = "type";
		return '<arg'.$i.' type="'.$type.'">'.$value.'</arg'.$i.'>'."\n";
	}
	else
	if(preg_match_all('/^(int|string|bool)@(\S*)$/',$arg,$match)){
		if($match[1][0]=="string"){
			// echo htmlspecialchars($match[2][0], ENT_XML1, 'UTF-8');
			return '<arg'.$i.' type="'.$match[1][0].'">'.htmlspecialchars($match[2][0], ENT_XML1, 'UTF-8').'</arg'.$i.'>'."\n";
		}else
		return '<arg'.$i.' type="'.$match[1][0].'">'.$match[2][0].'</arg'.$i.'>'."\n";
	}else
	if(preg_match_all('/^(LF|TF|GF)@(\S*)$/',$arg,$match)){
		return '<arg'.$i.' type="var">'.htmlspecialchars($match[0][0], ENT_XML1, 'UTF-8').'</arg'.$i.'>'."\n";
	}else{
		return '<arg'.$i.' type="label">'.$arg.'</arg'.$i.'>'."\n";
	}


	return '<arg'.$i.' type="'.$type.'">'.$value.'</arg'.$i.'>'."\n";
}

function write_help(){
	echo (
		"============================================================\n".
		"                  Parser pro jazyk IPPcode18\n".
		"------------------------------------------------------------\n".
		"Tento script nacte ze STDIN zdrojovy kod v jazyce IPPcode18,\n".
		"zkontroluje lexiklni a syntaktickou spravnost kodu a vypise \n".
		"na STDOUT XML reprezentaci programu.\n".
		"-------------------------------------------------------------\n".
		"Pouziti:\n".
		"	parse.php [parametry]\n".
		"Parametry:\n".
		"	--help	Vypise napovedu\n".
		"============================================================\n"
	);
}

?>

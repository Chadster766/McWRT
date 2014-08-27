BEGIN { FS=":"; nc = 0; ns = 0;
	if (USER != "root" && USER != "admin")
		config_load("webif_access_control")
}
/^##WEBIF:category:/ {
	if (categories !~ /:$3:/) {
		categories = categories ":" $3 ":";
		c[++nc] = $3
	}
	next;
}
/^##WEBIF:name:/ {
	url = FILENAME;
	gsub(/^.*\//, "", url);
	if (USER != "root" && USER != "admin" && $3 != "-") {
		if ($3 != "Graphs")
			allowed = config_get_bool(USER, $3"_"$4, "0")
		else	allowed = config_get_bool(USER, "Graphs", "0")
	}
	if ((USER == "root" || USER == "admin") || allowed == "1" || $3 == "-") {
		sp[$3,$5] = $4;
		st[$3,$5] = $5;
		sf[$3,$5] = rootdir "/" url;
		if ((p[$3] == 0) || (int($4) < p[$3])) {
			p[$3] = int($4) + 1;
			f[$3] = rootdir "/" url;
		}
	}
}
END {
	fname = cachedir "/cat_" USER;
	for (i = 1; i <= nc; i++) {
		if (c[i] == "-")
			print("-:-") > fname;

		else if (f[c[i]] != "")
			print(c[i] ":" f[c[i]]) > fname;
	}
	close(fname);

	fname = cachedir "/subcat_" USER;
	cmd = "sort -n >" fname;

	for (i = 1; i <= nc; i++) {
		if (f[c[i]] == "")
			continue;

		for (k in sf) {
			split(k, key, SUBSEP);
			if (key[1] == c[i])
				print(key[1] ":" sp[k] ":" st[k] ":" sf[k]) | cmd;
		}
	}
	close(cmd);
}

#!/usr/bin/webif-page
<?

. /usr/lib/webif/webif.sh
. /www/cgi-bin/webif/graphs-subcategories.sh

header_inject_head=$(cat <<EOF
    <script type="text/javascript" src="/webif.js"></script>
    <style type="text/css">
    <!--
    a.graphs_vnstat_link:visited {
        outline: none;
    }
    #graphs_vnstat_monthly {
        padding-top: 2px;
    }
    -->
    </style>
EOF
)

header "Graphs" "graphs_vnstat_subcategory#Vnstat" "" "" ""
has_pkgs vnstati

LIB_D=${IPKG_INSTROOT}/var/lib/vnstat

interfaces=$(ls $LIB_D)
if [ -z "$interfaces" ]; then
    echo "<pre>No database found, nothing to do. Use --help for help.</pre>"
    echo "<br />"
    echo "<pre>A new database can be created with the following command:</pre>"
    echo "<pre>    vnstat -u -i eth0</pre>"
    echo "<br />"
    echo "<pre>Replace 'eth0' with the interface that should be monitored. A list</pre>"
    echo "<pre>of available interfaces can be seen with the 'ifconfig' command.</pre>"
else
    BIN=${IPKG_INSTROOT}/usr/bin/vnstati
    VAR_D=${IPKG_INSTROOT}/var/vnstat
    WWW_D=${IPKG_INSTROOT}/www/vnstat

    [ -d $VAR_D ] || mkdir -p $VAR_D
    [ -d $WWW_D ] || mkdir -p $WWW_D

    interfaces_count=$(ls $LIB_D | wc -l)
    [ $interfaces_count -eq 1 ] && multiple=false || multiple=true

    outputs="s h d t m"
    $multiple && outputs="hs $outputs"

    for interface in $interfaces; do
        for output in $outputs; do
            [ -L $WWW_D/vnstat_${interface}_${output}.png ] || ln -sf $VAR_D/vnstat_${interface}_${output}.png $WWW_D/vnstat_${interface}_${output}.png
            $BIN -${output} -i $interface -c 15 -o $VAR_D/vnstat_${interface}_${output}.png
        done
cat <<EOF
    <h2>Traffic of Interface $interface</h2>
EOF
        if $multiple; then
cat <<EOF
    <a class="graphs_vnstat_link" href="#" title="Click to see ${interface}'s Details" onclick="set_visible('${interface}_summary', false); set_visible('${interface}_details', true);">
        <img id="${interface}_summary" src="/vnstat/vnstat_${interface}_hs.png" alt="${interface} Summary" />
    </a>
    <a class="graphs_vnstat_link" href="#" title="Click to see ${interface}'s Summary" onclick="set_visible('${interface}_details', false); set_visible('${interface}_summary', true);">
        <table id="${interface}_details" summary="${interface} Details" style="display: none;">
EOF
        else
cat <<EOF
    <table id="${interface}_details" summary="${interface} Details">
EOF
        fi
cat <<EOF
        <tbody>
            <tr>
                <td>
                    <img src="/vnstat/vnstat_${interface}_s.png" alt="${interface} Summary" />
                </td>
                <td>
                    <img src="/vnstat/vnstat_${interface}_h.png" alt="${interface} Hourly" />
                </td>
            </tr>
            <tr>
                <td valign="top">
                    <img src="/vnstat/vnstat_${interface}_d.png" alt="${interface} Daily" />
                </td>
                <td valign="top">
                    <img src="/vnstat/vnstat_${interface}_t.png" alt="${interface} Top 10" />
                    <br />
                    <img id="graphs_vnstat_monthly" src="/vnstat/vnstat_${interface}_m.png" alt="${interface} Monthly" />
                </td>
            </tr>
        </tbody>
EOF
        if $multiple; then
cat <<EOF
        </table>
    </a>

EOF
        else
cat <<EOF
    </table>

EOF
        fi
    done
fi

footer ?>
<!--
##WEBIF:name:Graphs:3:graphs_vnstat_subcategory#Vnstat
-->

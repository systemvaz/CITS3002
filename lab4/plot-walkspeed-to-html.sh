#!/bin/sh

#WALK_SPEED="0 1 2 3 4 5"
WALK_SPEED="0 1 2 3 4 5 6 7 8 9 10 11 12"

HTML="plot_walkspeed.html"

TITLE="Messages received for different walking speeds"`date '+,  run %c'`
XAXIS="seconds"

METRIC="messages received"
DURATION="1hour"
EVERY="20"

# location of the cnet simulator


CNET=cnet
# --  nothing needs changing below this line  --------------------------

run_simulations () {
    for w in `echo $WALK_SPEED`
    do
        rm -f stats.$w
        WALK_SPEED="$w" $CNET -WTqs -S 19 -e $DURATION -f ${EVERY}secs -N GEOROUTING    | \
        grep "$METRIC"                                          | \
        cut -d: -f 2                                            | \
        awk "{ printf(\"%d %s\n\", ++i * $EVERY, \$1); }"       > stats.$w
    done
}

collate_stats () {
    echo 'data.addRows(['

    T0=`echo " " $WALK_SPEED | sed -e 's/ / stats./g' | cut '-d ' -f3-`
    T1=`echo $T0 | cut '-d ' -f1`
    T2=`echo $T0 | cut '-d ' -f1,2`

    if [ "$T0" == "$T1" ]
    then
	CMD="cat $T0"
    elif [ "$T0" == "$T2" ]
    then
	CMD="join $T0"
    else
	CMD="join "`echo $T0 | cut '-d ' -f1`
	CMD="$CMD "`echo $T0 | cut '-d ' -f2- | sed 's/ / | join - /g'`
    fi

    eval $CMD | sed -e 's/ /, /g' -e 's/.*/[&],/'
    echo ']);'
}

add_titles () {
    echo "data.addColumn('number', '$XAXIS');"
    for w in `echo $WALK_SPEED`
    do
        echo "data.addColumn('number', '$w metres/sec');"
    done
}

build_html () {
    cat << END_END
<html>
<head>
  <meta http-equiv='refresh' content='10' />
  <script type='text/javascript' src='https://www.google.com/jsapi'></script>
  <script type='text/javascript'>
    google.load('visualization', '1.1', {packages: ['line']});
    google.setOnLoadCallback(drawChart);

    function drawChart() {
      var options = {
        chart: {
          title: '$TITLE'
        },
        width:  600,
        height: 400
      };
      var chart = new google.charts.Line(document.getElementById('linechart_material'));
      var data = new google.visualization.DataTable();
END_END

add_titles
collate_stats

cat << END_END
      chart.draw(data, options);
    }
  </script>
</head>
<body>
  <div id='linechart_material'></div>
</body>
</html>
END_END
}



rm -f $HTML
run_simulations
build_html > $HTML
echo "output is in $HTML"

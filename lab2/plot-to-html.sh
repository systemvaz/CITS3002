#!/bin/sh

# location of the cnet simulator
case `uname` in
  Linux)        CNET="/usr/bin/cnet"
                ;;

  Darwin)       export CNETPATH=".:/Volumes/cslinux/adhoc/lib/cnet"
                CNET="/Volumes/cslinux/bin/cnet-osx"
                ;;

  *)            echo "$0: cannot determine your architecture"
                exit 1
                ;;
esac

# some variables to define our simulation:
TOPOLOGY="STOPANDWAIT2_NEGACK"
METRIC="Average delivery time"
DURATION="7200s"
EVERY="10"

# some variables to define our output plot:
HTML="plot_snw2_negack_dtime.html"
TITLE="Average Delivery Time"
XAXIS="seconds"


# --  nothing needs changing below this line  --------------------------

if [ ! -x $CNET ]
then
    echo "$CNET not found or is not executable"
    exit 1
fi

function build_html   {

    cat > $HTML << END_END
<html>
<head>
<!--
  <meta http-equiv='refresh' content='10' />
-->
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

      data.addColumn('number', 'seconds');
      data.addColumn('number', '$TOPOLOGY');

      data.addRows([
END_END
# the actual simulation, and capturing of the statistics, are done here:

    $CNET -W -q -T -e $DURATION -s -f ${EVERY}secs $TOPOLOGY	| \
    grep "$METRIC"						| \
    cut -d: -f 2						| \
    awk "{ printf(\"        [%d, %s],\n\", ++i * $EVERY, \$1); }" | tee -a $HTML

cat >> $HTML << END_END
      ]);
      chart.draw(data, options);
    }
  </script>
</head>

<body>
  <div id='linechart_material'></div>
</body>
</html>
END_END}



rm -f $HTML
build_html
echo "output is in $HTML"

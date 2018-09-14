<html>
<head>
</head>
<body>

    <?php
        $servername = "localhost";
        $username = "root";
        $password = "";
        $dbname = "cmpe321hw3";

        // Create connection
        $conn = new mysqli($servername, $username, $password, $dbname);

        // Check connection
        if ($conn->connect_error) {
            die("Connection failed: " . $conn->connect_error);
        }else{

            // Insert the record
            $sql = "INSERT INTO task(ID, ProjectID, StartDate, DayItTakes) " .
                "VALUES(NULL, '" . $_POST['ProjectID'] . "', '" . $_POST['StartDate'] . "', " . $_POST['DayItTakes'] . ")";

            if ($conn->query($sql) === TRUE) {
                echo "Record was created successfully <br />";
                echo "<a href = 'list_task.php'>Go back</a>";
            } else {
                echo "Error updating record: " . $conn->error;
            }
        }
        $conn->close();
    ?>

</body>
</html>

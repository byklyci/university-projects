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

            // Delete the record
            $sql = "DELETE FROM manageproject WHERE ProjectID = " . $_POST['ProjectID'] . 
            " AND ProjectManagerID = " . $_POST['ProjectManagerID'];

            if ($conn->query($sql) === TRUE) {
                echo "Record was deleted successfully <br />";
                echo "<a href = 'list_manageproject.php'>Go back</a>";
            } else {
                echo "Error updating record: " . $conn->error;
            }
        }
        $conn->close();
    ?>

</body>
</html>

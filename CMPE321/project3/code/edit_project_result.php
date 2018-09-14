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

            // Update the record
            $sql = "UPDATE project SET PName = '" . $_POST['PName'] . "', StartDate = '" . $_POST['StartDate'] .
                "' WHERE ID = " . $_POST['ID'];

            if ($conn->query($sql) === TRUE) {
                echo "Record was updated successfully <br />";
                echo "<a href = 'list_project.php'>Go back</a>";
            } else {
                echo "Error updating record: " . $conn->error;
            }
        }
        $conn->close();
    ?>

</body>
</html>

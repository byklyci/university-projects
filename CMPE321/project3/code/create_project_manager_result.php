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
            $sql = "INSERT INTO user(ID, Username, Password, AdminRole) " .
                "VALUES(NULL, '" . $_POST['Username'] . "', '" . $_POST['Password'] . "', 0)";

            if ($conn->query($sql) === TRUE) {
                echo "Record was created successfully <br />";
                echo "<a href = 'list_project_manager.php'>Go back</a>";
            } else {
                echo "Error updating record: " . $conn->error;
            }
        }
        $conn->close();
    ?>

</body>
</html>

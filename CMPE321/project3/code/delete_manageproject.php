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

            // Fetch the record
            $sql = "SELECT ProjectID, ProjectManagerID FROM manageproject WHERE ProjectID = " . $_GET['projectid'] . " AND ProjectManagerID = " . $_GET['projectmanagerid'];
            $result = $conn->query($sql);

            // If the record actually exists
            if ($result->num_rows > 0) {
                ?>
                <form action="delete_manageproject_result.php" method="post">
                <?php

                // Get the data
                $row = $result->fetch_assoc();
                ?>
                    Are you sure you want to delete the following record? <br />
                    <p>Project ID: <input type="text" name="ProjectID" value = "<?php echo $row["ProjectID"] ?>" readonly /></p>
                    <p>Project Manager ID: <input type="text" name="ProjectManagerID" value = "<?php echo $row["ProjectManagerID"] ?>" readonly /></p>
                    <p><input type="submit" value = "Delete Record" /></p>
                </form>
                <?php
            } else {
                echo "Record does not exist";
            }
        }
        $conn->close();
    ?>

</body>
</html>

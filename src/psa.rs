use std::{fs::OpenOptions, io::{BufRead, BufReader, Seek, SeekFrom}, path::Path};

use anyhow::{Result, anyhow};

pub struct PSA {
    pub id: i32,
    pub version: String,
    pub header: String,
}

impl PSA {
    pub fn new() -> Self {
        let psa = PSA {
            id: 0,
            version: String::from(""),
            header: String::from(""),
        };
        return psa;
    }

    pub fn load_from_file(self, file: String) -> Result<()> {
        let path_to_file = Path::new(&file);
        if !path_to_file.exists() {
            return Err(anyhow!("File at '{}' does not exist or cannot be read", file));
        }
        let file = OpenOptions::new().read(true).open(path_to_file)?;

        let mut reader = BufReader::new(&file);
        reader.seek(SeekFrom::Start(0))?;

        for line in reader.lines() {
            let line = line?;

            todo!("Figure out how to read file");
        }

        return Ok(());
    }
}

import pandas as pd
from datetime import datetime
from openpyxl import load_workbook

# Path to your Excel file
excel_path = "/Users/Goku/Desktop/Application Files/job log.xlsx"

# Function to validate the Timing input
def validate_timing():
    while True:
        timing = input("Timing (E for Early, L for Late): ").strip().upper()
        if timing in ["E", "L"]:
            return timing
        else:
            print("Invalid input. Please enter 'E' for Early or 'L' for Late.")

# Function to add a new job application
def add_job_application(company, role, timing, status="Applied"):
    try:
        # Load existing data if the file exists
        df = pd.read_excel(excel_path)
    except FileNotFoundError:
        # Create a new DataFrame if the file does not exist
        df = pd.DataFrame(columns=["Application Date", "Company Name", "Role", "Status", "Timing", "Next Steps", "Notes"])

    # Add a new job application
    new_entry = {
        "Application Date": datetime.now().date(),
        "Company Name": company,
        "Role": role,
        "Status": status,
        "Timing": timing
    }
    df = pd.concat([df, pd.DataFrame([new_entry])], ignore_index=True)

    # Save the updated data back to Excel
    with pd.ExcelWriter(excel_path, engine="openpyxl", mode="w") as writer:
        df.to_excel(writer, index=False)
    
    print(f"Job application for {role} at {company} added successfully!")

# Main function to handle user inputs
if __name__ == "__main__":
    company = input("Company: ").strip()
    role = input("Role: ").strip()
    timing = validate_timing()
    add_job_application(company, role, timing)

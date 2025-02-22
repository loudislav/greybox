<?php


namespace App\Services;

use Fakturoid\FakturoidManager as FakturoidClient;
use Fakturoid\Response;
use GuzzleHttp\Client as GuzzleClient;

class FakturoidClientService // TODO: split into 2 services: subjects and invoices
{

    private $fakturoidClient;

    public function __construct()
    {
        $this->fakturoidClient = new FakturoidClient(
            new GuzzleClient(),
            env('FAKTUROID_CLIENT_ID'),
            env('FAKTUROID_CLIENT_SECRET'),
            env('FAKTUROID_USER_AGENT'),
            env('FAKTUROID_SLUG')
        );
        $this->fakturoidClient->authClientCredentials();
    }

    public function createSubject(Array $subjectData): Response
    {
        return $this->fakturoidClient->getSubjectsProvider()->create($subjectData);
    }

    public function getAllSubjects(): array
    {
        $subjects = array();
        $link = $this->fakturoidClient->getSubjects()->getHeader('Link');

        for ($page = 1; $page <= $this->getPagesCount($link); $page++)
        {
            $subjectsPage = $this->fakturoidClient->getSubjects(["page" => $page])->getBody();
            foreach ($subjectsPage as $subject)
            {
                array_push($subjects, $subject);
            }
        }
        return $subjects;
    }

    public function createInvoice(Array $invoiceData): Response
    {
        return $this->fakturoidClient->getInvoicesProvider()->create($invoiceData);
    }

    /*
     * Get array of all invoices from Fakturoid
     *
     * @return array
     */
    public function getAllInvoices(): array
    {
        $invoices = array();
        $lastUpdateTime = new \DateTime('- 2 days');
        $lastUpdateTimeString = $lastUpdateTime->format('Y-m-d\TH:i:s');

        $link = $this->fakturoidClient->getInvoices(['updated_since' => $lastUpdateTimeString])->getHeader('Link');

        for ($page = 1; $page <= $this->getPagesCount($link); $page++)
        {
            $invoicesPage = $this->fakturoidClient->getInvoices(['page' => $page, 'updated_since' => $lastUpdateTimeString])->getBody();
            foreach ($invoicesPage as $invoice)
            {
                array_push($invoices, $invoice);
            }
        }
        return $invoices;
    }

    private function getPagesCount($link = null): int
    {
        if (null !== $link) {
            $parts = explode('=', $link);
            preg_match('/[0-9]+/', $parts[1], $lastPage);
        } else {
            $lastPage[] = 1;
        }

        return $lastPage[0];
    }

    public function getInvoicePdf($fakturoidId)
    {
        return $this->fakturoidClient->getInvoicesProvider()->getPdf($fakturoidId);
    }
}